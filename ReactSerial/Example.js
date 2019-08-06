/* eslint no-console: 0 */
/* eslint react/prop-types: 0 */
/* eslint no-return-assign: 0 */
/* eslint import/no-named-as-default: 0 */
/* eslint import/no-named-as-default-member: 0 */

import React, { Component } from 'react';
import {
  InputGroup, InputGroupAddon, Button, Input,
} from 'reactstrap';
import withSerialCommunication from './SerialHOC';
import { WAKE_ARDUINO } from './arduinoConstants';
import IPC from './IPCMessages';
// import '../styles/index.css';

class Example extends Component {
  constructor(props) {
    super(props);
    this.state = {
      lastMessage: '...waiting for first message',
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
    this.setState({ lastMessage: JSON.stringify(data) });
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
    const { lastMessage } = this.state;
    const { ipcAvailable } = this.props;
    return (
      <div>
        <h1>Serial Example</h1>
        <hr />
        <br />
        <h2>
          <strong>IPC available:</strong>
          {' '}
          {ipcAvailable.toString()}
        </h2>

        <h2>
          <strong>Last message recieved:</strong>
          {' '}
          {lastMessage}
        </h2>
        <div style={{ display: 'flex', justifyContent: 'space-between' }}>
          <Button
            color="primary"
            onClick={() => this.sendClick(WAKE_ARDUINO)}
          >
          Wake Arduino
          </Button>
          <Button
            color="warning"
            onClick={() => this.sendClick(IPC.FLUSH_COMMAND)}
          >
            Flush
          </Button>
          <Button
            color="warning"
            onClick={() => this.sendClick(IPC.RESET_PORTS_COMMAND)}
          >
            Reset Ports
          </Button>

        </div>
        <div>
          <h2>Arduino messages:</h2>
          Wake arduino:
          <p>{'{"message":"wake-arduino", "value":1}'}</p>
          Write pwm value on (white LED):
          <p>{'{"message":"pwm-output", "value":150}'}</p>
          Write pwm value off (white LED):
          <p>{'{"message":"pwm-output", "value":0}'}</p>
          Open door (red LED):
          <p>{'{"message":"door-lock", "value":1}'}</p>
          Lock door (red LED):
          <p>{'{"message":"door-lock", "value":0}'}</p>
          Read analog input (will be floating):
          <p>{'{"message":"pot-rotation", "value":1}'}</p>
          Push button:
          <p>{'{"message":"vrs-button-press", "value":1}'}</p>
          <InputGroup>
            <InputGroupAddon addonType="prepend">
              <Button
                color="primary"
                onClick={() => this.sendClick(this.sendTextInput.value)}
              >
                Send:
              </Button>
              <Input
                innerRef={input => (this.sendTextInput = input)}
                placeholder="{“message”:”wake-arduino”, “value”:1}"
              />
            </InputGroupAddon>
          </InputGroup>
        </div>
      </div>
    );
  }
}

const ExampleWithSerialCommunication = withSerialCommunication(Example);

export default ExampleWithSerialCommunication;
