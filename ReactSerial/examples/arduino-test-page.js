import React, { Component } from 'react';
import withSerialCommunication from '../SerialHOC';
import { WAKE_ARDUINO } from '../ArduinoConstants';
import IPC from '../IPCMessages';

class ArduinoTestPage extends Component {
  constructor(props) {
    super(props);
    this.state = {
      lastMessage: '...waiting on the first message',
      log: [],
      logCount: 0,
    };

    this.onData = this.onData.bind(this);
    this.sendClick = this.sendClick.bind(this);

    this.logLimit = 150;
    this.logArray = [];
    this.sendTextInput = React.createRef();
    this.logContainer = React.createRef();
  }

  componentDidMount() {
    const { setOnDataCallback } = this.props;
    setOnDataCallback(this.onData);
  }

  componentDidUpdate() {
    // Scroll to the bottom of the log container when new logs are added.
    this.logContainer.current.scrollTop = this.logContainer.current.scrollHeight;
  }

  onData(data) {
    console.log('onData:', data);
    const { logCount } = this.state;
    const timestamp = Date.now();
    const logData = {
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
    const { sendData } = this.props;
    sendData(msg);
  }

  render() {
    const { lastMessage, log, logCount } = this.state;
    const { ipcAvailable } = this.props;
    return (
      <div style={{ padding: '3%' }}>
        <h1>Arduino Test Page</h1>
        <p>
          Don&apos;t know what this is?{' '}
          <a href="https://github.com/scimusmn/arduino-base">Read docs here.</a>
        </p>
        <br />
        <h3>
          <strong>IPC available:</strong>{' '}
          <span style={{ color: ipcAvailable.toString() === 'false' ? 'red' : 'green' }}>
            {ipcAvailable.toString()}
          </span>
        </h3>
        <h3>
          <strong>Last message received:</strong>{' '}
          <span style={{ color: lastMessage === '...waiting on first message' ? 'gray' : 'green' }}>
            {lastMessage}
          </span>
        </h3>
        <h3>
          <strong>Incoming message count:</strong>{' '}
          <span style={{ color: logCount === 0 ? 'gray' : 'green' }}>{logCount}</span>
        </h3>
        <br />
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <button
            style={{ backgroundColor: 'teal', color: 'white', border: 'none', padding: '10px' }}
            onClick={() => this.sendClick(WAKE_ARDUINO)}
          >
            Wake Arduino
          </button>
        </div>
        <br />
        <div>
          <div>
            <button
              style={{ backgroundColor: 'teal', color: 'white', border: 'none', padding: '10px' }}
              onClick={() => this.sendClick(this.sendTextInput.current.value)}
            >
              Send:
            </button>
            {` `}
            <input
              ref={this.sendTextInput}
              placeholder="{wake-arduino:1}"
              style={{ padding: '10px' }}
            />
          </div>
        </div>
        <br />
        <div>
          <button
            style={{ backgroundColor: 'orange', color: 'white', border: 'none', padding: '10px' }}
            onClick={() => this.sendClick(IPC.FLUSH_COMMAND)}
          >
            Flush
          </button>
          {' '}
          <button
            style={{ backgroundColor: 'orange', color: 'white', border: 'none', padding: '10px' }}
            onClick={() => this.sendClick(IPC.RESET_PORTS_COMMAND)}
          >
            Reset Ports
          </button>
        </div>
        <br />
        <hr />
        <h4>Log</h4>
        <div
          ref={this.logContainer}
          style={{
            height: '278px',
            boxSizing: 'border-box',
            overflowY: 'scroll',
            border: '#ddd solid 1px',
            backgroundColor: '#efefef',
            padding: '10px',
          }}
        >
          {log.map((logItem) => (
            <div key={logItem.id}>{logItem.content}</div>
          ))}
        </div>
        <span style={{ fontSize: '12px', color: logCount < this.logLimit ? 'gray' : 'red' }}>
          History limit: {log.length}/{this.logLimit}
        </span>
      </div>
    );
  }
}

const ArduinoTestPageWithSerialCommunication = withSerialCommunication(ArduinoTestPage);

export default ArduinoTestPageWithSerialCommunication;
