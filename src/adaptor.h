/*
  Aerostat Beam Coder - Node.js native bindings for FFmpeg.
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
    T val = 0;
    if (!qu.empty()) {
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
    mActive = false;
    if ((0 == qu.size()) || (qu.size() >= mMaxQueue)) {
      // ensure release of any blocked thread
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
    : mBufRef(bufRef), mBuf(buf), mLen(bufLen), mLocalAlloc(nullptr == bufRef) {}
  ~Chunk() { if (mLocalAlloc) free(mBuf); }

  napi_ref buf_ref() const  { return mBufRef; }
  const void *buf() const  { return mBuf; }
  size_t len() const  { return mLen; }

private:
  const napi_ref mBufRef;
  void *mBuf;
  const size_t mLen;
  const bool mLocalAlloc;
};

class Adaptor {
public:
  Adaptor(uint32_t queueLen)
    : mQueue(new Queue<Chunk *>(queueLen)), mCurChunk(nullptr), mChunkPos(0), m(), mBuf(1024) {}
  ~Adaptor() {
    delete mQueue;
    std::unique_lock<std::mutex> lk(m);
    while (mDone.size()) {
      Chunk *chunk = mDone.back();
      mDone.pop_back();
      delete chunk;
    }
    mDone.clear();
  }

  int write(const uint8_t *buf, int bufSize) {
    uint8_t *qBuf = (uint8_t *)malloc(bufSize);
    memcpy(qBuf, buf, bufSize);
    mQueue->enqueue(new Chunk(nullptr, qBuf, bufSize));
    return bufSize;
  }

  void *read(size_t numBytes, size_t *bytesRead) {
    uint8_t *buf = (uint8_t *)malloc(numBytes);
    *bytesRead = fillBuf(buf, numBytes);
    if (numBytes != *bytesRead) {
      if (0 == *bytesRead) {
        free(buf);
        buf = nullptr;
      }
      else
        buf = (uint8_t *)realloc(buf, *bytesRead);
    }
    return buf;
  }

  void write(napi_ref bufRef, void *buf, size_t bufLen) {
    mQueue->enqueue(new Chunk(bufRef, buf, bufLen));
  }

  int read(uint8_t *buf, int bufSize) {
    return fillBuf(buf, bufSize);
  }

  void finish()  { mQueue->quit(); }

  napi_status finaliseBufs(napi_env env) {
    napi_status status = napi_ok;
    std::unique_lock<std::mutex> lk(m);
    while (mDone.size()) {
      Chunk *chunk = mDone.back();
      mDone.pop_back();
      if (chunk->buf_ref())
        status = napi_delete_reference(env, chunk->buf_ref());
      delete chunk;
      if (napi_ok != status) break;
    }
    return status;
  }

  // convenience buffer for avio_alloc_context
  //  - it shouldn't be needed but avformat_write_header crashes if no buffer is provided
  unsigned char *buf()  { return &mBuf[0]; }
  int bufLen() const  { return (int)mBuf.size(); }

private:
  Queue<Chunk *> *mQueue;
  std::vector<Chunk *> mDone;
  Chunk *mCurChunk;
  size_t mChunkPos;
  mutable std::mutex m;
  std::vector<unsigned char> mBuf;

  int fillBuf(uint8_t *buf, size_t numBytes) {
    int bufOff = 0;
    while (numBytes) {
      if (!mCurChunk || (mCurChunk && mCurChunk->len() == mChunkPos))
        if (!nextChunk())
          break;

      int curSize = FFMIN(numBytes, mCurChunk->len() - mChunkPos);
      void *srcBuf = (uint8_t *)mCurChunk->buf() + mChunkPos;
      memcpy(buf + bufOff, srcBuf, curSize);

      bufOff += curSize;
      mChunkPos += curSize;
      numBytes -= curSize;
    }

    return bufOff;
  }

  bool nextChunk() {
    if (mCurChunk) {
      std::unique_lock<std::mutex> lk(m);
      mDone.push_back(mCurChunk);
    }

    mCurChunk = mQueue->dequeue();
    mChunkPos = 0;
    return nullptr != mCurChunk;
  }
};

#endif
