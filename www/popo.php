<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="Refresh" content="1; URL=http://localhost:1998">
	<title></title>
</head>
<body>

<?php
$name = $_POST['name'];
$phone = $_POST['phone'];
$email = $_POST['email'];
$name = htmlspecialchars($name);
$phone = htmlspecialchars($phone);
$email = htmlspecialchars($email);
$name = urldecode($name);
$phone = urldecode($phone);
$email = urldecode($email);
$name = trim($name);
$phone = trim($phone);
$email = trim($email);
//echo $fio;
//echo "<br>";
//echo $email;
if (mail("makhmudego@yandex.ru", "Заявка с сайта", "ФИО: ".$name. ". телефон: ".$phone.". E-mail: ".$email ,"From: makhmudego@yandex.ru \r\n"))
 {     echo "сообщение успешно отправлено"; 
} else { 
    echo "при отправке сообщения возникли ошибки";
}?>
</body>
</html>