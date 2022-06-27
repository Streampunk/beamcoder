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

import test from 'tape';
import beamcoder from '..';

test('Creating a decoder', t => {
  let dec = beamcoder.decoder({ name: 'h264' });
  t.ok(dec, 'is truthy.');
  t.equal(dec.name, 'h264', 'has the expected name.');
  t.equal(dec.codec_id, 27, 'has the expected codec_id.');
  t.ok(typeof dec._CodecContext == 'object', 'external value present.');
  t.equal(dec.type, 'decoder', 'has expected type name.');
  t.end();
});

test('Checking the A properties:', t => {
  let dec = beamcoder.decoder({ name: 'h264' });

  t.deepEqual(dec.active_thread_type, { FRAME: false, SLICE: false},
    'active_thread_type has expected default.');
  // @ts-expect-error:next-line
  t.throws(() => { dec.active_thread_type = { FRAME: true }; }, /User cannot/,
    'active_thread_type cannot be set.');

  t.equals(dec.apply_cropping, 1, 'apply_cropping has exepceted default.');
  t.doesNotThrow(() => { dec.apply_cropping = 0; },
    'apply_cropping setting does not throw.');
  t.equals(dec.apply_cropping, 0, 'value has been updated.');

  t.equals(dec.audio_service_type, 'main',
    'audio_service_type has expected default value.');
  // @ts-expect-error:next-line
  t.throws(() => { dec.audio_service_type = 'dialogue'; },
    /decoding/, 'cannot be updated when deocoding.');

  t.end();
});

// TODO properties B to Z
