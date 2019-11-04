# NativeWebp
A library to make animated webp picture on Android based on libwep(https://developers.google.com/speed/webp), support for adding 
picture frames and composeing output an animated webp picture file.</br>

## Use as below:
Import module animwebp or animwebp-V1.0.0.aar into your project. Then : </br>
```Java
AnimWebPMaker maker=new AnimWebPMaker();
maker.setFrameDuration(100);
maker.setLoopCount(0);
maker.setQuality(80);
maker.setMixed(true);
maker.setOutputPath(outputPath);
//Add some image frames from local path, but only support webp files.
maker.addImage(path);
...
//Or Add from memory, support all format.
maker.addImage(AnimWebPMaker.bitmap2Array(bitmap));
...
maker.make();

//Or just call static method, only support webp files.
AnimWebPMaker.makeOnce(pathArray, true, 0, 100, 80, outputPath);
```
