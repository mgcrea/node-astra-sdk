const through = require('through2');
const astra = require('astra-sdk').default;
const DepthStream = require('astra-sdk').DepthStream;
const {app, io} = require('./server');

const log = console.log.bind(console);

astra.initialize((err, streamReader) => {
  const depthStream = new DepthStream(streamReader, {/*frequency: 300*/});
  depthStream.on('error', (err) => {
    log('Encountered stream error', err);
    astra.terminate();
  });
  depthStream
    .pipe(through.obj((frame, encoding, next) => {
      const {index, delay, width, height, data, min, max} = frame;
      log('Emitting new frame widh index=%d, payload=%j', index, {width, height, min, max});
      for (let i = 0; i < data.length; i++) {

      }
      io.sockets.volatile.emit('frame', {index, delay, width, height, /* data, */min, max, createdAt: new Date()});
      next();
    }))
    .on('end', () => {
      log('Stream ended');
    });
});
