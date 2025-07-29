import { fileURLToPath } from 'url';
import { join, dirname } from 'path';
import { createRequire } from 'module';

const __dirname = dirname(fileURLToPath(import.meta.url));
const require = createRequire(import.meta.url);
const addon = require(join(__dirname, '../build/Release/addon.node'));

export function hello(): string {
  return addon.hello();
}
