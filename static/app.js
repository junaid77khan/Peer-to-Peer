// let ws = null;
// let myPeerId = null;
// let peerConnections = {};

// let localStream;
// let peerConnection;
// let websocket;
// let localPeerId;
// let remotePeerId;

// const configuration = {
//     iceServers: [
//         { urls: 'stun:stun.l.google.com:19302' },
//         { urls: 'stun:stun1.l.google.com:19302' }
//     ]
// };

let ws = null;
let myPeerId = null;
let peerConnections = {};

let localStream;
let peerConnection;
let websocket;
let localPeerId;
let remotePeerId;

const configuration = {
    iceServers: [
        { urls: 'stun:stun.l.google.com:19302' },
        { urls: 'stun:stun1.l.google.com:19302' }
    ]
};

// function updateStatus(message) {
//     document.getElementById('status').textContent = message;
// }

function updateStatus(message) {
    document.getElementById('status').textContent = message;
}

// function updatePeerList(peers) {
//     // Filter out our own peer ID from the list
//     const filteredPeers = peers.filter(peerId => peerId !== localPeerId);
    
//     const peerList = document.getElementById('peerList');
//     peerList.innerHTML = '<h3>Available Peers:</h3>';
    
//     if (filteredPeers.length === 0) {
//         peerList.innerHTML += '<p>No other peers available</p>';
//         return;
//     }
    
//     filteredPeers.forEach(peerId => {
//         const button = document.createElement('button');
//         button.textContent = `Connect to ${peerId}`;
//         button.className = 'peer-button';
//         button.onclick = () => connectToPeer(peerId);
//         peerList.appendChild(button);
//     });
// }

// function connectWebSocket() {
//     const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
//     websocket = new WebSocket(`${protocol}//${window.location.host}/ws`);

//     websocket.onmessage = async function(event) {
//         const message = JSON.parse(event.data);
//         console.log('Received message:', message);

//         switch(message.type) {
//             case 'peer_id':
//                 localPeerId = message.id;
//                 console.log('Received local peer ID:', localPeerId);
//                 updateStatus(`Your ID: ${localPeerId}`);
//                 break;

//             case 'peer_list':
//                 updatePeerList(message.peers);
//                 break;

//             case 'connection_request':
//                 handleConnectionRequest(message.from);
//                 break;

//             case 'offer':
//                 await handleOffer(message);
//                 break;

//             case 'answer':
//                 await handleAnswer(message);
//                 break;

//             case 'ice-candidate':
//                 handleIceCandidate(message);
//                 break;
//         }
//     };

//     websocket.onopen = () => {
//         updateStatus('Connected to server');
//     };

//     websocket.onclose = () => {
//         updateStatus('Disconnected from server');
//     };

//     websocket.onerror = (error) => {
//         updateStatus('WebSocket error: ' + error.message);
//         console.error('WebSocket error:', error);
//     };
// }

// async function handleAnswer(message) {
//     try {
//         await peerConnection.setRemoteDescription(new RTCSessionDescription(message.sdp));
//         updateStatus('Connection established with peer');
//     } catch (e) {
//         console.error('Error handling answer:', e);
//         updateStatus('Error establishing connection');
//     }
// }

// async function handleOffer(message) {
//     try {
//         if (!peerConnection) {
//             createPeerConnection();
//         }

//         await peerConnection.setRemoteDescription(new RTCSessionDescription(message.sdp));
//         const answer = await peerConnection.createAnswer();
//         await peerConnection.setLocalDescription(answer);

//         const responseMessage = {
//             type: 'answer',
//             sdp: answer,
//             target: message.from
//         };
//         websocket.send(JSON.stringify(responseMessage));
//         updateStatus('Answering call from peer');
//     } catch (e) {
//         console.error('Error handling offer:', e);
//         updateStatus('Error handling incoming call');
//     }
// }

function updatePeerList(peers) {
    // Filter out our own peer ID and any already connected peers
    const filteredPeers = peers.filter(peerId => {
        return peerId !== localPeerId && !peerConnections[peerId];
    });
    
    const peerList = document.getElementById('peerList');
    peerList.innerHTML = '<h3>Available Peers:</h3>';
    
    if (filteredPeers.length === 0) {
        peerList.innerHTML += '<p>No other peers available</p>';
        return;
    }
    
    filteredPeers.forEach(peerId => {
        const button = document.createElement('button');
        button.textContent = `Connect to ${peerId}`;
        button.className = 'peer-button';
        button.onclick = () => connectToPeer(peerId);
        peerList.appendChild(button);
    });

    // Add a section for connected peers
    const connectedPeers = Object.keys(peerConnections);
    if (connectedPeers.length > 0) {
        const connectedSection = document.createElement('div');
        connectedSection.innerHTML = '<h3>Connected Peers:</h3>';
        connectedPeers.forEach(peerId => {
            const peerDiv = document.createElement('div');
            peerDiv.textContent = peerId;
            peerDiv.className = 'connected-peer';
            connectedSection.appendChild(peerDiv);
        });
        peerList.appendChild(connectedSection);
    }
}

function connectWebSocket() {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    websocket = new WebSocket(`${protocol}//${window.location.host}/ws`);

    websocket.onmessage = async function(event) {
        const message = JSON.parse(event.data);
        console.log('Received message:', message);

        switch(message.type) {
            case 'peer_id':
                localPeerId = message.id;
                console.log('Received local peer ID:', localPeerId);
                updateStatus(`Your ID: ${localPeerId}`);
                break;

            case 'peer_list':
                updatePeerList(message.peers);
                break;

            case 'connection_request':
                showConnectionRequest(message.from);
                break;

            case 'offer':
                await handleOffer(message);
                break;

            case 'answer':
                await handleAnswer(message);
                break;

            case 'ice-candidate':
                handleIceCandidate(message);
                break;
        }
    };

    websocket.onopen = () => {
        updateStatus('Connected to server');
    };

    websocket.onclose = () => {
        updateStatus('Disconnected from server');
    };

    websocket.onerror = (error) => {
        updateStatus('WebSocket error: ' + error.message);
        console.error('WebSocket error:', error);
    };
}

async function handleAnswer(message) {
    try {
        await peerConnection.setRemoteDescription(new RTCSessionDescription(message.sdp));
        // Add to peer connections after successful answer
        peerConnections[message.from] = peerConnection;
        updateStatus('Connection established with peer');
        // Request updated peer list
        websocket.send(JSON.stringify({ type: 'get_peers' }));
    } catch (e) {
        console.error('Error handling answer:', e);
        updateStatus('Error establishing connection');
    }
}

async function handleOffer(message) {
    try {
        if (!peerConnection) {
            createPeerConnection();
        }

        await peerConnection.setRemoteDescription(new RTCSessionDescription(message.sdp));
        const answer = await peerConnection.createAnswer();
        await peerConnection.setLocalDescription(answer);

        const responseMessage = {
            type: 'answer',
            sdp: answer,
            target: message.from
        };
        websocket.send(JSON.stringify(responseMessage));
        // Add to peer connections after creating answer
        peerConnections[message.from] = peerConnection;
        updateStatus('Answering call from peer');
        // Request updated peer list
        websocket.send(JSON.stringify({ type: 'get_peers' }));
    } catch (e) {
        console.error('Error handling offer:', e);
        updateStatus('Error handling incoming call');
    }
}

async function startStream() {
    try {
        localStream = await navigator.mediaDevices.getUserMedia({
            video: true,
            audio: true
        });
        document.getElementById('localVideo').srcObject = localStream;
        updateStatus('Local stream started');
    } catch (e) {
        console.error('Error accessing media devices:', e);
        updateStatus('Error accessing camera/microphone');
    }
}

function stopStream() {
    if (localStream) {
        localStream.getTracks().forEach(track => track.stop());
        document.getElementById('localVideo').srcObject = null;
        localStream = null;
        
        // Clean up peer connections
        if (peerConnection) {
            peerConnection.close();
            peerConnection = null;
        }
        
        document.getElementById('remoteVideo').srcObject = null;
        updateStatus('Stream stopped');
    }
}

async function connectToPeer(peerId) {
    if (!localStream) {
        updateStatus('Please start your video stream first');
        return;
    }

    remotePeerId = peerId;
    createPeerConnection();

    try {
        const offer = await peerConnection.createOffer();
        await peerConnection.setLocalDescription(offer);

        const message = {
            type: 'offer',
            sdp: offer,
            target: remotePeerId
        };
        websocket.send(JSON.stringify(message));
        updateStatus('Sending connection request...');
    } catch (e) {
        console.error('Error creating offer:', e);
        updateStatus('Error connecting to peer');
    }
}

async function handleIceCandidate(message) {
    try {
        if (message.candidate) {
            await peerConnection.addIceCandidate(new RTCIceCandidate(message.candidate));
        }
    } catch (e) {
        console.error('Error handling ICE candidate:', e);
    }
}

function createPeerConnection() {
    if (peerConnection) {
        peerConnection.close();
    }

    peerConnection = new RTCPeerConnection(configuration);

    peerConnection.onicecandidate = event => {
        if (event.candidate) {
            const message = {
                type: 'ice-candidate',
                candidate: event.candidate,
                target: remotePeerId
            };
            websocket.send(JSON.stringify(message));
        }
    };

    peerConnection.ontrack = event => {
        console.log('Received remote track');
        document.getElementById('remoteVideo').srcObject = event.streams[0];
        updateStatus('Connected to peer');
    };

    peerConnection.oniceconnectionstatechange = () => {
        updateStatus(`ICE Connection State: ${peerConnection.iceConnectionState}`);
    };

    // Add local stream
    if (localStream) {
        localStream.getTracks().forEach(track => {
            peerConnection.addTrack(track, localStream);
        });
    }
}

async function handleConnectionRequest(fromPeerId) {
    if (confirm(`Accept connection from peer ${fromPeerId}?`)) {
        remotePeerId = fromPeerId;
        if (!localStream) {
            await startStream();
        }
        createPeerConnection();
        updateStatus('Accepting connection request...');
    }
}

async function handleConnectionResponse(accepted, fromPeerId) {
    const modal = document.querySelector('.modal');
    if (modal) {
        modal.remove();
    }

    if (accepted) {
        remotePeerId = fromPeerId;
        if (!localStream) {
            await startStream();
        }
        createPeerConnection();
        // Add this peer to our connections
        peerConnections[fromPeerId] = peerConnection;
        updateStatus('Accepting connection request...');
    } else {
        // Send rejection message if needed
        updateStatus('Connection request rejected');
    }
}

function showConnectionRequest(fromPeerId) {
    const modal = document.createElement('div');
    modal.className = 'modal';
    modal.innerHTML = `
        <div class="modal-content">
            <h3>Connection Request</h3>
            <p>Peer ${fromPeerId} wants to connect with you.</p>
            <div class="modal-buttons">
                <button onclick="handleConnectionResponse(true, '${fromPeerId}')" class="accept-button">Accept</button>
                <button onclick="handleConnectionResponse(false, '${fromPeerId}')" class="reject-button">Reject</button>
            </div>
        </div>
    `;
    document.body.appendChild(modal);
}

document.addEventListener('DOMContentLoaded', () => {
    // Initialize WebSocket connection
    connectWebSocket();
    
    // Add classes to the control buttons
    const buttons = {
        startStreamBtn: 'Start Stream',
        stopStreamBtn: 'Stop Stream',
        showPeersBtn: 'Show Peers'
    };

    Object.entries(buttons).forEach(([id, text]) => {
        const button = document.getElementById(id);
        button.className = 'control-button';
        button.textContent = text;
    });

    // Add event listeners
    document.getElementById('startStreamBtn').addEventListener('click', startStream);
    document.getElementById('stopStreamBtn').addEventListener('click', stopStream);
    document.getElementById('showPeersBtn').addEventListener('click', () => {
        websocket.send(JSON.stringify({ type: 'get_peers' }));
    });
});