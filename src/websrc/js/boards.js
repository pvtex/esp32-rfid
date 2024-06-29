function handleBoard() {
    var bType = parseInt(document.getElementById("boardtype").value)
    if (bType === 1) { // generic
      document.getElementById("numrlys").value = "1";
      document.getElementById("changenumrly").click();
      document.getElementById("gpiorly"). value = "16";
      document.getElementById("accessdeniedpin").value = "255";
      document.getElementById("beeperpin").value = "13";
      document.getElementById("doorstatpin").value = "14";
      document.getElementById("ledwaitingpin").value = "12";
      document.getElementById("openlockpin").value = "15";
      document.getElementById("wifipin").value = "255";
      document.getElementById("wg0pin").value = "4";
      document.getElementById("wg1pin").value = "5";
    } else if (bType === 2) { // APWIKOGER Relay X1
      document.getElementById("numrlys").value = "1";
      document.getElementById("changenumrly").click();
      document.getElementById("gpiorly"). value = "16";
      document.getElementById("accessdeniedpin").value = "255";
      document.getElementById("beeperpin").value = "13";
      document.getElementById("doorstatpin").value = "17";
      document.getElementById("ledwaitingpin").value = "14";
      document.getElementById("openlockpin").value = "39";
      document.getElementById("wifipin").value = "255";
      document.getElementById("wg0pin").value = "33";
      document.getElementById("wg1pin").value = "26";
    } else if (bType === 3) { // DTWonder DT-R002
      document.getElementById("numrlys").value = "2";
      document.getElementById("changenumrly").click();
      document.getElementById("gpiorly"). value = "16";
      document.getElementById("gpiorly2"). value = "2";
      document.getElementById("accessdeniedpin").value = "255";
      document.getElementById("beeperpin").value = "3";
      document.getElementById("doorstatpin").value = "36";
      document.getElementById("ledwaitingpin").value = "1";
      document.getElementById("openlockpin").value = "39";
      document.getElementById("wifipin").value = "255";
      document.getElementById("wg0pin").value = "4";
      document.getElementById("wg1pin").value = "5";
    } else if (bType === 4) { // LILYGO t-internet-com
      document.getElementById("numrlys").value = "1";
      document.getElementById("changenumrly").click();
      document.getElementById("gpiorly"). value = "32";
      document.getElementById("accessdeniedpin").value = "255";
      document.getElementById("beeperpin").value = "15";
      document.getElementById("doorstatpin").value = "33";
      document.getElementById("ledwaitingpin").value = "13";
      document.getElementById("openlockpin").value = "35";
      document.getElementById("wifipin").value = "255";
      document.getElementById("wg0pin").value = "14";
      document.getElementById("wg1pin").value = "2";
    }
  }