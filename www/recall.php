<?
	$name = $_POST['name'];
	$phone = $_POST['phone'];
	$email = $_POST['email'];

	$pdo = new PDO('mysql:host=localhost;dbname=makhmudego_1', 'makhmudego_1', '412365');
	$stmt = $pdo->prepare("INSERT INTO recalls(`time`, `name`, `phone`, `email`) VALUES(NOW(), '" . $name . "',  '" . $phone . "', '" . $email . "');");
	$stmt->execute();

	echo 'OK';
