/*
  Aerostat Beam Engine - Redis-backed highly-scale-able and cloud-fit media beam engine.
  Copyright (C) 2019  Streampunk Media Ltd.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  https://www.streampunk.media/ mailto:furnace@streampunk.media
  14 Ormiscaig, Aultbea, Achnasheen, IV22 2JJ  U.K.
*/

#ifndef ADAPTOR_H
#define ADAPTOR_H

#include "node_api.h"
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class Queue {
public:
  Queue(uint32_t maxQueue) : mActive(true), mMaxQueue(maxQueue), qu(), m(), cv() {}
  ~Queue() {}
  
  void enqueue(T t) {
    std::unique_lock<std::mutex> lk(m);
    while(mActive && (qu.size() >= mMaxQueue)) {
      cv.wait(lk);
    }
    qu.push(t);
    cv.notify_one();
  }
  
  T dequeue() {
    std::unique_lock<std::mutex> lk(m);
    while(mActive && qu.empty()) {
      cv.wait(lk);
    }
    T val;
    if (mActive) {
      val = qu.front();
      qu.pop();
      cv.notify_one();
    }
    return val;
  }

  size_t size() const {
    std::lock_guard<std::mutex> lk(m);
    return qu.size();
  }

  void quit() {
    std::lock_guard<std::mutex> lk(m);
    if ((0 == qu.size()) || (qu.size() >= mMaxQueue)) {
      // ensure release of any blocked thread
      mActive = false;
      cv.notify_all();
    }
  }

private:
  bool mActive;
  uint32_t mMaxQueue;
  std::queue<T> qu;
  mutable std::mutex m;
  std::condition_variable cv;
};

class Chunk {
public:
  Chunk(napi_ref bufRef, void *buf, size_t bufLen)
    : mBufRef(bufRef), mBuf(buf), mLen(bufLen) {}
  ~Chunk() {}

  napi_ref buf_ref() const  { return mBufRef; }
  const void *buf() const  { return mBuf; }
  size_t len() const  { return mLen; }

private:
  const napi_ref mBufRef;
  const void *mBuf;
  const size_t mLen;
};

class Adaptor {
public:
  Adaptor(Queue<Chunk *> *queue)
    : mQueue(queue), mCurPos(0), mCurChunk(nullptr), mChunkPos(0), mFinished(false), m() {}
  ~Adaptor() {
    delete mQueue;
    mDone.clear();
  }

  void write(Chunk *chunk) {
    mQueue->enqueue(chunk);
  }

  int read(uint8_t *buf, int bufSize) {
    int bufOff = 0;
    while (bufSize) {
      if (!mCurChunk || (mCurChunk && mCurChunk->len() == mChunkPos))
        if (!nextChunk())
          break;

      int curSize = FFMIN(bufSize, mCurChunk->len() - mChunkPos);
      void *srcBuf = (uint8_t *)mCurChunk->buf() + mChunkPos;
      memcpy(buf + bufOff, srcBuf, curSize);

      bufOff += curSize;
      mChunkPos += curSize;
      bufSize -= curSize;
    }

    return bufOff;
  }

  void finish() {
    mFinished = true;
  }

  napi_status finaliseBufs(napi_env env) {
    napi_status status = napi_ok;
    std::unique_lock<std::mutex> lk(m);
    while (mDone.size()) {
      Chunk *chunk = mDone.back();
      mDone.pop_back();
      status = napi_delete_reference(env, chunk->buf_ref());
      delete chunk;
      if (napi_ok != status) break;
    }
    return status;
  }

private:
  Queue<Chunk *> *mQueue;
  std::vector<Chunk *> mDone;
  size_t mCurPos;
  Chunk *mCurChunk;
  size_t mChunkPos;
  bool mFinished;
  mutable std::mutex m;

  bool nextChunk() {
    if (mCurChunk) {
      std::unique_lock<std::mutex> lk(m);
      mDone.push_back(mCurChunk);
    }

    bool result = true;
    if (mFinished && (0 == mQueue->size()))
      result = false;
    else {
      mCurChunk = mQueue->dequeue();
      mChunkPos = 0;
    }
    return result;
  }
};

#endif
