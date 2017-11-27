import expect from 'expect';
import mongoose from 'mongoose';

import {DeptStream} from './../../src';

describe('Plugin', () => {
  it('constructor should export a function', () => {
    expect(typeof DeptStream).toBe('function');
  });
});
