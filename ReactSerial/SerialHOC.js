/* eslint no-console: 0 */
/* eslint max-len: 0 */
/* eslint react/jsx-props-no-spreading: 0 */
import React from 'react';
import ReactScrollableList from 'react-scrollable-list';
import _ from 'lodash';
import queryString from 'query-string';
import IPC from './IPCMessages';

const withSerialCommunication = (WrappedComponent) => {
  class HOC extends React.Component {
    constructor(props) {
      super(props);

      this.state = {
        ipcAvailable: false,
        debugView: false,
        logs: [],
      };

      this.ipcRenderer = null;

      this.checkIpcAvailability = this.checkIpcAvailability.bind(this);
      this.sendData = this.sendData.bind(this);
      this.setOnDataCallback = this.setOnDataCallback.bind(this);
      this.startIpcCommunication = this.startIpcCommunication.bind(this);
      this.stopIpcCommunication = this.stopIpcCommunication.bind(this);
      this.onSerialData = this.onSerialData.bind(this);
    }

    componentDidMount() {
      // Check whether to display debug overlay
      const currentQueryString = queryString.parse(window.location.search);
      this.setState({ debugView: (_.get(currentQueryString, 'debug', false) === 'true') });
      this.startIpcCommunication();
    }

    componentWillUnmount() {
      this.stopIpcCommunication();
    }

    onSerialData(event, arg) {
      let data = {};

      this.logLine(`onSerialData: ${JSON.stringify(arg)}`);

      try {
        // Strip arduino carriage return
        // and split key / value
        const keyValueArray = arg.replace(/[{}\s]/gm, '').split(':');
        data = {
          [keyValueArray[0]]: keyValueArray[1],
        };
      } catch (err) {
        this.logLine(`WARNING: Unable to parse serial data:${err}`);
      }

      if (this.onDataCallback) {
        this.onDataCallback(data);
      } else {
        this.logLine(`No onDataCallback set. Unused data:${data}`);
      }
    }

    setOnDataCallback(callback) {
      this.onDataCallback = callback;
    }

    sendData(data) {
      const { ipcAvailable } = this.state;

      this.logLine('From sendData HOC: ', data);
      this.logLine('ipcAvailable: ', ipcAvailable);

      if (ipcAvailable) {
        this.logLine(`sendData: ${data}`);
        this.ipcRenderer.send(IPC.RENDERER_TO_SERIAL, data);
      } else {
        this.logLine(`IPC not available: ${data}`);
      }
    }

    startIpcCommunication() {
      this.ipcRenderer = this.checkIpcAvailability();
      if (this.ipcRenderer === null) return;

      // Listen for all pass-through serial events from Electron
      this.ipcRenderer.on(IPC.SERIAL_TO_RENDERER, this.onSerialData);

      // Tell Electron's main process you
      // want to open communication.
      this.ipcRenderer.send(IPC.HANDSHAKE, window.location.href);
    }

    stopIpcCommunication() {
      this.logLine('stopIpcCommunication');
      if (this.ipcRenderer) {
        this.ipcRenderer.removeAllListeners();
        this.ipcRenderer = null;
      }
    }

    checkIpcAvailability() {
      if (window.ipcRef) {
        this.logLine('checkIpcAvailability: ipcAvailable: true');
        this.setState({ ipcAvailable: true });
        return window.ipcRef;
      }

      this.logLine('WARNING: window.ipcRef unavailable. Make sure you are setting window.ipcRef in index.html:');
      this.logLine('checkIpcAvailability: ipcAvailable: false');

      this.setState({ ipcAvailable: false });
      return null;
    }

    logLine(data) {
      const { debugView } = this.state;
      if (debugView) {
        const { logs } = this.state;
        const log = {
          id: `log-${logs.length}`,
          content: data,
        };
        this.setState({ logs: [...logs, log] });
      }
    }

    render() {
      const { ipcAvailable, logs, debugView } = this.state;
      const logStyles = {
        color: '#FFFE2B',
        fontFamily: 'Lucida Console, Monaco, monospace',
        fontSize: '12px',
        position: 'fixed',
        top: 0,
        left: 0,
        padding: '30px',
        backgroundColor: 'rgba(0, 0, 0, 0.8)',
        width: '490px',
        height: '240px',
        boxSizing: 'border-box',
        overflowY: 'scroll',
        border: '#00FFBF solid 1px',
      };

      let debugOverlay = null;
      if (debugView) {
        debugOverlay = (
          <ReactScrollableList
            listItems={logs}
            heightOfItem={30}
            maxItemsToRender={200}
            style={logStyles}
          />
        );
      }

      return (
        <>
          <WrappedComponent
            sendData={this.sendData}
            setOnDataCallback={this.setOnDataCallback}
            ipcAvailable={ipcAvailable}
            startIpcCommunication={this.startIpcCommunication}
            stopIpcCommunication={this.stopIpcCommunication}
            {...this.props}
          />
          {debugOverlay}
        </>
      );
    }
  }

  return HOC;
};

export default withSerialCommunication;
