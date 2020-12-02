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
import withSerialCommunication from '../Arduino/arduino-base/ReactSerial/SerialHOC';
import { WAKE_ARDUINO } from '../Arduino/arduino-base/ReactSerial/ArduinoConstants';
import IPC from '../Arduino/arduino-base/ReactSerial/IPCMessages';

class ArduinoPage extends Component {
  constructor(props) {
    super(props);
    this.state = {
      lastMessage: '...waiting on first message',
      log: [],
    };

    this.onData = this.onData.bind(this);
    this.sendClick = this.sendClick.bind(this);
  }

  componentDidMount() {
    console.log('Serial Example mounted');
    const { setOnDataCallback } = this.props;
    setOnDataCallback(this.onData);
  }

  onData(data) {
    console.log('onData:', data);
    const timestamp = Date.now();
    const logData = {
      id: timestamp,
      content: `${timestamp} - ${JSON.stringify(data)}`,
    };
    const { log } = this.state;
    this.setState({ lastMessage: JSON.stringify(data), log: [...log, logData] });
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
    const { lastMessage, log } = this.state;
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
          listItems={log}
          heightOfItem={30}
          maxItemsToRender={2000}
          style={{
            minHeight: '220px',
            boxSizing: 'border-box',
            overflowY: 'scroll',
            overflowAnchor: 'none',
            border: '#ddd solid 1px',
            backgroundColor: '#efefef',
          }}
        />
      </div>
    );
  }
}

const ArduinoPageWithSerialCommunication = withSerialCommunication(ArduinoPage);

export default ArduinoPageWithSerialCommunication;
