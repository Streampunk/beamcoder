/*
  Aerostat Beam Coder - Node.js native bindings to FFmpeg
  Copyright (C) 2019 Streampunk Media Ltd.
  Copyright (C) 2022 Chemouni Uriel.
  
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
export interface ffStats {
    mean: number;
    stdDev: number;
    max: number;
    min: number;
}
export default function calcStats<K extends string, P extends string>(arr: Array<{ [key in K]: { [prop in P]: number } }>, elem: K, prop: P): ffStats {
    const values: number[] = arr.filter(cur => cur[elem]).map(cur => cur[elem][prop]);
    const mean: number = values.reduce((acc, cur) => acc + cur, 0) / arr.length;
    const max: number = Math.max(...values)
    const min: number = Math.min(...values)
    // standard deviation
    const sumDelta: number = values.reduce((acc, cur) => acc + Math.pow(cur - mean, 2), 0);
    const stdDev: number = Math.pow(sumDelta / arr.length, 0.5);
    return { mean, stdDev, max, min };
  };
  
  