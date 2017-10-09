<?php
echo "<br>tzLib is currently using pecl.php <b>timezonedb-" . timezone_version_get() . "</b><br><br>";
echo "Running \"pecl list-upgrades\" ...<br><b>";
echo exec("pecl list-upgrades") . "</b><br><br>";
echo "If an upgrade is available, the following command will apply it:<br>";
echo "&emsp; sudo pecl upgrade timezonedb";
?>