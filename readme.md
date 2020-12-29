# castLabs test sample readme

This is the test application to parse *.mp4 test file, print the structure of the file. As a bonus, it decodes and writes embedded images to disk.

The project completed in Xcode on the MacOS.

## Usage

./castLabs text0.mp4


## Output sample

Successfully loaded file text0.mp4
File size is: 18089
Found box of type moof and size 181
Found box of type mfhd and size 16
Found box of type traf and size 157
Found box of type tfhd and size 24
Found box of type trun and size 20
Found box of type uuid and size 44
Found box of type uuid and size 61
Found box of type mdat and size 17908
The image size in XML: 5246 bytes
File name is: image001.PNG
The image size in XML: 5630 bytes
File name is: image002.PNG
The image size in XML: 5630 bytes
File name is: image003.PNG
Content of mdat block is: <?xmlversion="1.0"encoding="UTF-8"?><ttxml:lang="spa"xmlns="http://www.w3.org/ns/ttml"xmlns:tts="http://www.w3.org/ns/ttml#styling"xmlns:smpte="http://www.smpte-ra.org/schemas/2052-1/2010/smpte-tt"><head><smpte:informationsmpte:mode="Enhanced"/><styling><stylexml:id="emb"tts:fontSize="4.1%"tts:fontFamily="monospaceSansSerif"/><stylexml:id="ttx"tts:fontSize="3.21%"tts:fontFamily="monospaceSansSerif"/><stylexml:id="backgroundStyle"tts:fontFamily="proportionalSansSerif"tts:fontSize="18px"tts:textAlign="center"tts:origin="0%66%"tts:extent="100%33%"tts:backgroundColor="rgba(0,0,0,0)"tts:displayAlign="center"/><stylexml:id="speakerStyle"style="backgroundStyle"tts:color="white"tts:textOutline="black1px"tts:backgroundColor="transparent"/><stylexml:id="textStyle"style="speakerStyle"tts:color="white"tts:textOutline="none"tts:backgroundColor="black"/></styling><layout><regionxml:id="full"tts:origin="0%0%"tts:extent="100%100%"tts:zIndex="1"/><regionxml:id="speaker"style="speakerStyle"tts:zIndex="1"/><regionxml:id="backgro
We done!


NOTE: the base64 decoding code taken from the Internet.
