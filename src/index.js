
import bindings from 'bindings';

import DepthStream from './streams/DepthStream';

const astra = bindings('addon.node');

let _streamReader;
let _isExiting = false;

const gracefulExit = () => {
	if (_isExiting) {
		return;
	}
	_isExiting = true;
	api.terminate(() => {
		process.exit(0);
	});
}

const api = {
	initialize: (callback = () => {}) => {
		astra.initialize((err, streamReader) => {
			if (!err) {
				_streamReader = streamReader;
				process.on('SIGINT', gracefulExit);
				process.on('SIGTERM', gracefulExit);
			}
			callback(err, streamReader);
		});
	},
	terminate: (callback = () => {}) => {
		if (_streamReader) {
			_streamReader.close(() => {
				astra.terminate(callback);
			});
		} else {
			astra.terminate(callback);
		}
	}
}

export {DepthStream};
export default api;
