<!DOCTYPE html>
<html>
	<head>
		<title>Todo list</title>
		<link rel="stylesheet" href="css/style.css">
	</head>
	<body>
		<div id="page">
			<a href="about.php">About</a>
			<p class="error">
			<?php
				if (strlen($db->error))
					echo $db->error;
			?>
			</p>
			<h1>Best todo list ever</h1>
			<form action="/task/create" method="POST">
				<label for="task">
					Enter your task
				</label>
				<input id="task" type="text" name="task">
				<input type="submit" value="submit" class="submit">
			</form>

			<div>
				<?php
					if (isset($db->data[$ses]) and (count($db->data[$ses]) > 1 or !isset($db->data[$ses][0])))
					{
						foreach ($db->data[$ses] as $key => $id)
						{
							$task = $db->data[$ses][$key];
							if ($key != 0)
								echo "<div class=\"task\">
					<p>$task</p>
					<form action=\"/task/delete\" method=\"POST\">
						<input type=\"hidden\" name=\"task\" value=\"$key\">
						<input class=\"delete\" type=\"submit\" value=\"delete\">
					</form>
				</div>
				";
						}
					}
					else
						echo "<h1>Welcome</h1>";
				?>
			</div>
		</div>
	</body>
</html>
