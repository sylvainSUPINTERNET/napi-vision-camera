import { hello, loadCtiFile } from './dist/index.js';
console.log(hello()); // "Hello from C++!"

loadCtiFile('test.cti');
