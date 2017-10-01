const mmap = require('../')
const fs = require('fs')
const assert = require('assert')

const fd = fs.openSync('tmp.txt', 'w+')

fs.ftruncate(fd, 20, (err) => {

  assert.ifError(err);
  console.log(fs.readFileSync('tmp.txt', 'utf8'))
  
  console.log("Mapping 20 bytes from file with file descriptor %d.", fd)
  let buf = mmap.mmap(20, mmap.PROT_WRITE, mmap.MAP_SHARED, fd, 0)

  console.log(buf)
  console.log("Unmapping...")

  let ret = mmap.munmap(buf, 20)

  console.log("Return Value %d.", ret)
})


