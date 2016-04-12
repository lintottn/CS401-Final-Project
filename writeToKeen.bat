:: @ECHO OFF

SET collection=group
SET projectID=57099d2c46f9a74e76dd2688
SET writeKey=XXX

FOR /r %%i IN (*.txt) DO REN *.txt *.json

FOR /r %%i IN (*.json) DO curl "https://api.keen.io/3.0/projects/%projectID%/events/%collection%?api_key=%writeKey%" -H "Content-Type: application/json" -d @%%~ni.json



