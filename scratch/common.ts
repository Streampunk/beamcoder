import fs, { Stats } from 'fs';
import path from 'path';
import url from 'url';

export function getMedia(fileName: string, asURL?: boolean): string {
    return `../../media/${fileName}`;
    //let media = path.join('..', 'media');
    //let stats: Stats | null = null;
    //try {
    //    stats = fs.statSync(media);
    //} catch (e) {
    //    // ignore
    //}
    //if (stats && !stats.isDirectory()) {
    //    media = path.join('..', '..', 'media');
    //    stats = fs.statSync(media);
    //}
    //if (stats && !stats.isDirectory()) {
    //    media = path.join('..', '..', '..', 'media');
    //    stats = fs.statSync(media);
    //} 
    //if (stats && !stats.isDirectory()) {
    //    throw Error(`media directory not found`);
    //}
    //const ret = path.resolve(media, fileName);
    //if (fs.existsSync(ret))
    //    throw Error(`missing ${ret} file`);
    //    
    //// return ret.replace(/\\/g, '/');
    //if (asURL)
    //    return url.pathToFileURL(ret).toString();
    //else
    //    return ret.replace(/\\/g, '/');
}