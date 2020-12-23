/* eslint no-console: 0 */
/* eslint react/prop-types: 0 */
/* eslint no-return-assign: 0 */
/* eslint import/no-named-as-default: 0 */
/* eslint import/no-named-as-default-member: 0 */

import React, { Component } from 'react';
import {
  InputGroup, InputGroupAddon, Button, Input,
} from 'reactstrap';
import ReactScrollableList from 'react-scrollable-list';
import withSerialCommunication from '../SerialHOC';
import { WAKE_ARDUINO } from '../ArduinoConstants';
import IPC from '../IPCMessages';

class ArduinoTestPage extends Component {
  constructor(props) {
    super(props);
    this.state = {
      lastMessage: '...waiting on first message',
      log: [],
      logCount: 0,
    };

    this.onData = this.onData.bind(this);
    this.sendClick = this.sendClick.bind(this);

    this.logLimit = 150;
    this.logArray = [];
  }

  componentDidMount() {
    const { setOnDataCallback } = this.props;
    setOnDataCallback(this.onData);
  }

  onData(data) {
    console.log('onData:', data);
    const { logCount } = this.state;
    const timestamp = Date.now();
    const logData = {
      // Generate unique ID
      id: `${logCount}-${timestamp}`,
      content: `${logCount} - ${timestamp} - ${JSON.stringify(data)}`,
    };

    this.logArray.push(logData);

    if (this.logArray.length > this.logLimit) this.logArray.shift();

    this.setState({
      lastMessage: JSON.stringify(data),
      log: this.logArray,
      logCount: logCount + 1,
    });
  }

  sendClick(msg) {
    console.log('sendClick:', msg);

    // This is where we pass it through
    // our HOC method to Stele, which passes
    // to Serial device.
    const { sendData } = this.props;
    sendData(msg);
  }

  render() {
    const { lastMessage, log, logCount } = this.state;
    const { ipcAvailable } = this.props;
    return (
      <div style={{ padding: '5%' }}>
        <h1>Arduino Test Page</h1>
        <span>
          Don&apos;t know what this is?
          {' '}
          <a href="https://github.com/scimusmn/arduino-base">Read docs here.</a>
        </span>
        <hr />
        <br />
        <h3>
          <strong>IPC available:</strong>
          {' '}
          <span style={{ color: ipcAvailable.toString() === 'false' ? 'red' : 'green' }}>{ipcAvailable.toString()}</span>
        </h3>
        <br />
        <h3>
          <strong>Last message recieved:</strong>
          {' '}
          <span style={{ color: lastMessage === '...waiting on first message' ? 'gray' : 'green' }}>{lastMessage}</span>
        </h3>
        <br />
        <h3>
          <strong>Incoming message count:</strong>
          {' '}
          <span style={{ color: logCount === 0 ? 'gray' : 'green' }}>{logCount}</span>
        </h3>
        <br />
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <Button
            color="primary"
            onClick={() => this.sendClick(WAKE_ARDUINO)}
          >
            Wake Arduino
          </Button>
        </div>
        <br />
        <div>
          <InputGroup>
            <InputGroupAddon addonType="prepend">
              <Button
                color="primary"
                onClick={() => this.sendClick(this.sendTextInput.value)}
              >
                Send:
              </Button>
              <Input
                innerRef={(input) => (this.sendTextInput = input)}
                placeholder="{wake-arduino:1}"
              />
            </InputGroupAddon>
          </InputGroup>
        </div>
        <br />
        <div>
          <Button
            color="warning"
            onClick={() => this.sendClick(IPC.FLUSH_COMMAND)}
          >
            Flush
          </Button>
          {' '}
          {' '}
          <Button
            color="warning"
            onClick={() => this.sendClick(IPC.RESET_PORTS_COMMAND)}
          >
            Reset Ports
          </Button>
        </div>
        <br />
        <hr />
        <h4>
          Log
        </h4>
        <ReactScrollableList
          listItems={this.logArray}
          heightOfItem={30}
          maxItemsToRender={this.logLimit}
          style={{
            height: '278px',
            boxSizing: 'border-box',
            overflowY: 'scroll',
            overflowAnchor: 'none',
            border: '#ddd solid 1px',
            backgroundColor: '#efefef',
          }}
        />
        <span style={{ fontSize: '12px', color: logCount < this.logLimit ? 'gray' : 'red' }}>
          History limit:
          {' '}
          {log.length}
          /
          {this.logLimit}
        </span>
      </div>
    );
  }
}

const ArduinoTestPageWithSerialCommunication = withSerialCommunication(ArduinoTestPage);

export default ArduinoTestPageWithSerialCommunication;
