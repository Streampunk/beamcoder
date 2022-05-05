import https from 'https';

/**
 * @param ws writable stream to pipe datastream
 * @param url url to download
 * @param name name to display in screen
 * @returns Promise
 */
export async function getRaw(ws: NodeJS.WritableStream, url: string, name?: string): Promise<void> {
    let received = 0;
    let totalLength = 0;
    if (!name)
        name = new URL(url).pathname.replace(/.*\//g, '')
    return new Promise((comp, err) => {
      https.get(url, res => {
        if (res.statusCode === 301 || res.statusCode === 302) {
          err({ name: 'RedirectError', message: res.headers.location });
        } else {
          res.pipe(ws);
          if (totalLength == 0) {
            totalLength = +(res.headers['content-length'] as string);
          }
          res.on('end', () => {
            process.stdout.write(`Downloaded 100% of '${name}'. Total length ${received} bytes.\n`);
            comp();
          });
          res.on('error', err);
          res.on('data', x => {
            received += x.length;
            process.stdout.write(`Downloaded ${received * 100/ totalLength | 0 }% of '${name}'.\r`);
          });
        }
      }).on('error', err);
    });
  }
  
export async function getHTML(url: string, name: string): Promise<Buffer> {
    let received = 0;
    let totalLength = 0;
    return new Promise((resolve, reject) => {
      https.get(url, res => {
        const chunks: Array<Uint8Array> = [];
        if (totalLength == 0) {
          totalLength = +(res.headers['content-length'] as string);
        }
        res.on('end', () => {
          process.stdout.write(`Downloaded 100% of '${name}'. Total length ${received} bytes.\n`);
          resolve(Buffer.concat(chunks));
        });
        res.on('error', reject);
        res.on('data', (chunk) => {
          chunks.push(chunk);
          received += chunk.length;
          process.stdout.write(`Downloaded ${received * 100/ totalLength | 0 }% of '${name}'.\r`);
        });
      }).on('error', reject);
    });
  }
  
  