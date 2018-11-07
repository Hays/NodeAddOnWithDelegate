const addon = require('bindings')('demoaddon')

addon.register(function(...params) {
  console.log("callback from native, parameters: ", params)
})

addon.test1()
let ret = addon.test2()
console.log("call test2 return ", ret)
addon.test3("Hello")
addon.test4(3, "Hays", true)
setTimeout(() => {
  console.log('finish app')
}, 10000)