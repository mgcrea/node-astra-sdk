import {Readable} from 'stream';

export default class DepthStream extends Readable {
  static defaults = {
    frequency: 33.34, // 30Hz
    timeout: 2000, // 2s
  }
  state = {
    source: null,
    isReady: false
  }
  setState = (update) => { this.state = {...this.state, ...update}}
	constructor(streamReader, options = {}) {
    super({
      objectMode: true
    });
    this.options = {...DepthStream.defaults, ...options};
    this.setState({source: streamReader});
		this.state.source.startDepthStream((err, info) => {
			if (err) {
				this.emit('error', err);
				return;
			}
      this.setState({isReady: true});
		})
	}
  close(callback = () => {}) {
    this.state.source.close(callback);
  }
  _read() {
    const {isReady, source} = this.state;
    const {frequency, timeout} = this.options;
		// StreamReader not ready
		if (!isReady) {
			setTimeout(() => {
				this._read();
			}, frequency);
			return;
		}
		source.getDepthFrame({x: '123', timeout, min: true, select: ['min', 'max']}, (err, frame) => {
			if (err) {
				this.emit('error', err);
				return;
			}
			setTimeout(() => {
				this.push(frame);
			}, frequency);
		})
	}
}
