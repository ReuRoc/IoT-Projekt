let color_test = require('./color_test.js');

color_test.init();

function testfunc(){
    color_test.postColor();
    setTimeout(() => {
        testfunc()
    }, 1000);;
}
