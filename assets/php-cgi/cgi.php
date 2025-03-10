<?php

class Db
{
	public	$sess;
	public	$data;
	public	$error;
	
	function __construct()
	{
		$this->data = [];
		$this->error = "";
		if (!($file = fopen("assets/php-cgi/.data.csv", "a+")))
		{
			$this->error = "cannot read the data";
			return ;
		}
		$this->sess = 0;
		while (($line = fgets($file)) != false)
		{
			$id = 0;
			$ses = 0;
			$task = "";
			sscanf($line, "%d,%d,%[^\n]", $ses, $id, $task);
			if (!isset($this->data[$ses]))
				$this->data[$ses] = [];
			if ($ses > $this->sess)
				$this->sess = $ses;
			$this->data[$ses][$id] = $task;
		}
		fclose($file);
	}

	function __destruct()
	{
		$file = fopen("assets/php-cgi/.data.csv", "w");
		if (!$file)
		{
			$this->error = "cannot write the data";
			return ;
		}
		foreach ($this->data as $key => $ses)
		{
			foreach ($ses as $key2 => $id)
			{
				fprintf($file, "%d,%d,%s\n", $key, $key2, $ses[$key2]);
			}
		}
		fclose($file);
	}

	function	addData($ses, $task)
	{
		if (!isset($this->data[$ses]))
			$this->data[$ses] = [];
		$id = count($this->data[$ses]) + 1;
		$this->data[$ses][$id] = $task;
	}

	function addDataFromBody($ses, $body)
	{
		if ($ses == -1)
			$ses = $this->sess + 1;
		$task = "";
		sscanf($body, "task=%[^\n]", $task);
		$task = urldecode($task);
		$this->addData($ses, $task);
		return ($ses);
	}

	function setUser($ses)
	{
		if ($ses == -1)
			$ses = $this->sess + 1;
		if(!isset($this->data[$ses]))
		{
			$this->data[$ses] = [];
			$this->data[$ses][0] = "";
		}
		return ($ses);
	}

	function deleteDataFromBody($ses, $body)
	{
		if ($ses == -1)
			$ses = $this->setUser($ses);
		$task = 0;
		sscanf($body, "task=%d", $task);
		if (isset($this->data[$ses]) and isset($this->data[$ses][$task]))
			unset($this->data[$ses][$task]);
		return ($ses);
	}

	function	removeData($ses, $id)
	{
		unset($this->data[$ses][$id]);
	}
}

$db = new Db();

$ses = -1;

$method = getenv("METHOD");
$cookies = getenv("COOKIE");
$pathInfo = getenv("PATH_INFO");
$body = getenv("BODY");

$found = false;

if ($cookies)
{
	foreach (explode("; ", $cookies) as $key => $val)
	{
		if (explode("=", $val)[0] == "session_id")
		{
			sscanf($val, "session_id=%d", $ses);
			$found = true;
			if (!isset($db->data[$ses]))
			{
				$found = false;
				$ses = -1;
			}
		}
	}
}

if ($method == "POST" and $pathInfo == "/task/create")
{
	if ($body)
		$ses = $db->addDataFromBody($ses, $body);
	echo "HTTP/1.1 303 See Other\n";
	echo "Date: " . gmdate('D, d M Y H:i:s') . " GMT\n";
	echo "Location: /redirect\n";
	echo "Content-Length: 0\n";
	if (!$found)
		echo "Set-Cookie: session_id=" . $ses . "\n";
	echo "\r\n";
	return ;
}

if ($method == "POST" and $pathInfo == "/task/delete")
{
	if ($body)
		$ses = $db->deleteDataFromBody($ses, $body);
	echo "HTTP/1.1 303 See Other\n";
	echo "Date: " . gmdate('D, d M Y H:i:s') . " GMT\n";
	echo "Location: /redirect\n";
	echo "Content-Length: 0\n";
	if (!$found)
		echo "Set-Cookie: session_id=" . $ses . "\n";
	echo "\r\n";
	return ;
}

$ses = $db->setUser($ses);

ob_start();

if ($pathInfo == "/about.php")
	include("about.php");
elseif ($pathInfo == "/" or $pathInfo == "/index.php")
	include("index.php");
else
{
	include("404.php");
	$content = ob_get_clean();
	$length = strlen($content);

	echo "HTTP/1.1 404 Not Found\n";
	echo "Content-Type: text/html\n";
	echo "Content-Length: " . $length . "\n";
	echo "Date: " . gmdate('D, d M Y H:i:s') . " GMT\n";
	if (!$found)
		echo "Set-Cookie: session_id=" . $ses . "\n";
	echo "\r\n";
	echo $content;
	return ;
}

$content = ob_get_clean();
$length = strlen($content);

echo "HTTP/1.1 200 OK\n";
echo "Content-Type: text/html\n";
echo "Content-Length: " . $length . "\n";
echo "Date: " . gmdate('D, d M Y H:i:s') . " GMT\n";
if (!$found)
	echo "Set-Cookie: session_id=" . $ses . "\n";

echo "\r\n";
echo $content;


/*
	- env -i METHOD="GET" PATH_INFO="/" php cgi.php
	- env -i METHOD="GET" PATH_INFO="/test" php cgi.php
	- env -i METHOD="GET" PATH_INFO="/about.php" php cgi.php
	- env -i METHOD="GET" PATH_INFO="/" COOKIE="session_id=0" php cgi.php
	- env -i METHOD="DELETE" PATH_INFO="/task" COOKIE="session_id=0" BODY="task=1" php cgi.php
	- env -i METHOD="POST" PATH_INFO="/task" COOKIE="session_id=0" BODY="task=hi there" php cgi.php
*/

?>