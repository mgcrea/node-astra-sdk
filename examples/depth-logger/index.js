const through = require('through2');
const astra = require('astra-sdk').default;
const DepthStream = require('astra-sdk').DepthStream;

const log = console.log.bind(console);

astra.initialize((err, streamReader) => {
	const depthStream = new DepthStream(streamReader);
	depthStream.on('error', (err) => {
		log('Encountered stream error', err);
		astra.terminate();
	});
  depthStream
    .pipe(through.obj((frame, encoding, next) => {
      const {index, delay, width, height, data, min, max} = frame;
      log('Received new frame:', {index, delay, width, height, data, min, max});
      next();
    }))
    .on('end', () => {
  		log('Stream ended');
  	});
});
