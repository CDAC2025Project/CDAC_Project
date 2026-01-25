// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyBJWKKQkyOJFT4PjntJ9R_ehr_AHdmiqHE",
  authDomain: "unit-89d9b.firebaseapp.com",
  databaseURL: "https://unit-89d9b-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "unit-89d9b",
  storageBucket: "unit-89d9b.firebasestorage.app",
  messagingSenderId: "1022422303208",
  appId: "1:1022422303208:web:86a83247fc7398dd32a45d",
  measurementId: "G-S23D092MMV"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Reference to the monitoring data
const dataRef = ref(database, 'monitoring_unit');

// Listen for data updates
onValue(dataRef, (snapshot) => {
  const data = snapshot.val();
  
  if (data) {
    // Update Temperature
    document.getElementById("temp").innerText = (data.temperature ?? "--") + " °C";

    // Update Humidity
    document.getElementById("hum").innerText = (data.humidity ?? "--") + " %";

    // Update Pressure
    document.getElementById("pressure").innerText = (data.pressure ?? "--") + " Pa";
    
    console.log("Data received:", data);
  } else {
    console.log("No data available at 'monitoring_unit'");
  }
}, (error) => {
  console.error("Error fetching data:", error);
});
