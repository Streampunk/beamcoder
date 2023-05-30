const {execSync} = require('child_process');

if (process.env.PREBUILD) {
  execSync('npx patch-package');
}
