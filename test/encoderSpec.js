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

const test = require('tape');
const beamcoder = require('../index.js');

test('Creating a video encoder', t => {
  let enc = beamcoder.encoder({ name: 'h264' });
  t.ok(enc, 'is truthy.');
  t.equal(enc.name, 'libx264', 'has the expected name.');
  t.equal(enc.codec_id, 27, 'has the expected codec_id.');
  t.ok(typeof enc._CodecContext == 'object', 'external value present.');
  t.equal(enc.type, 'encoder', 'has expected type name.');
  t.end();
});

test('Creating an audio encoder', t => {
  let enc = beamcoder.encoder({ name: 'aac' });
  t.ok(enc, 'is truthy.');
  t.equal(enc.name, 'aac', 'has the expected name.');
  t.equal(enc.codec_id, 86018, 'has the expected codec_id.');
  t.ok(typeof enc._CodecContext == 'object', 'external value present.');
  t.equal(enc.type, 'encoder', 'has expected type name.');
  t.end();
});

test('Checking the A properties:', t => {
  let enc = beamcoder.encoder({ name: 'h264' });

  t.deepEqual(enc.active_thread_type, { FRAME: false, SLICE: false},
    'active_thread_type has expected default.');
  t.throws(() => { enc.active_thread_type = { FRAME: true }; }, /User cannot/,
    'active_thread_type cannot be set.');

  t.notOk(enc.apply_cropping, 'apply_cropping not defined for encoding.');
  t.throws(() => { enc.apply_cropping = 0; }, /encoding/,
    'apply_cropping setting does not throw.');

  t.equals(enc.audio_service_type, 'main',
    'audio_service_type has expected default value.');
  t.doesNotThrow(() => { enc.audio_service_type = 'dialogue'; },
    'audio_service_type can be updated.');
  t.equals(enc.audio_service_type, 'dialogue',
    'audio_service_type has been updated.');
  t.throws(() => { enc.audio_service_type = 'wibble'; },
    'audio_service_type throws with unknown value.');

  t.end();
});

// TODO properties B to Z
