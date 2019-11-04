# NativeWebp
A library to make animated webp picture on Android based on libwep(https://developers.google.com/speed/webp), support for adding webp
picture frames and composeing output an animated webp picture file.</br>

##Use as below:
```Java
AnimWebPMaker maker=new AnimWebPMaker();
maker.setFrameDuration(100);
maker.setLoopCount(0);
maker.setQuality(80);
maker.setMixed(true);
maker.setOutputPath(outputPath);
//Add some image frames from local path
maker.addImage(path);
...
//Or Add from memory
maker.addImage(AnimWebPMaker.bitmap2Array(bitmap));
...
maker.make();

//Or just
AnimWebPMaker.makeOnce(pathArray, true, 0, 100, 80, outputPath);
```
