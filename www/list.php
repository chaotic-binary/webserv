<!DOCTYPE html>
<html lang="ru">
<head>
	<title>Заявки</title>
	<meta charset="utf-8">
	<style>
		ul {
			list-style: none;
			color: white;
			font-size: 18px;
			padding: 0;
			background-color: #f9ababb5;

		}

		li {
			margin-bottom: 25px;
			font-family: sans-serif;
			padding: 5px 20px;
			background-color: #ff4e2e;	
			box-shadow: 0 0 10px #ffffff;		
		}
		span {
			display: inline-block;
		}
		.id {
			width: 30px;
			border-right: 2px solid #ff4e2e;
		}
		.time {
			padding-left: 20px;
			width: 200px;
		}
		.name {
			width: 200px;
		}
		.phone {
			width: 150px;
		}
		.email {
			width: 200px;
		}
		body {
			padding: 0;
			margin: 0;
			display: flex;
			justify-content: center;
		}

	</style>
</head>
<body>
<?php
$servername = "localhost";
$username = "makhmudego_1";
$password = "412365";
$dbname = "makhmudego_1";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, time, name, phone, email FROM recalls";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row?>
    <ul>
    	<li>
    		<span class="id">Id</span>
    		<span class="time">Time</span>
    		<span class="name">Name</span>
    		<span class="phone">Phone</span>
    		<span class="email">Email</span>
    	</li>
    <?while($row = $result->fetch_assoc()) { ?>

    	<li><? echo "<span class='id'>" . $row["id"] . "</span>" . "<span class='time'>" . $row["time"] . "</span>" .  "<span class='name'>" . $row["name"] . "</span>" . "<span class='phone'>" . $row["phone"] . "</span>" . "<span class='email'>" . $row["email"] . "</span>";?>
    	</li>

    <?}
} else {
    echo "0 results";
}
$conn->close();
?>
</ul>

</body>
</html>