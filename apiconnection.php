<?php

$servername = "localhost";
$username = "root";
$password = "";

// Create connection
$conn = new mysqli($servername, $username, $password);

try {
    $conn = new PDO("mysql:host=$servername;dbname=iot test", $username, $password);
    // set the PDO error mode to exception
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "INSERT INTO `mpu6050` (`sensor`, `button`, `senior_id`) VALUES ( :sensor, :button, 1)";
    // use exec() because no results are returned
    $stmt = $conn->prepare($sql);
    
    $sensor = (int)$_GET["sensor"];
    $button = (int)$_GET["button"];
    $stmt->bindParam(':sensor', $sensor);
    $stmt->bindParam(':button', $button);
    $stmt->execute();
    
    echo "New record created successfully";
    }
catch(PDOException $e)
    {
    echo "Connection failed: " . $e->getMessage();
    }

     

?>
