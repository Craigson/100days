//THIS TUMBLR BOT SEARCHES FOR POSTS WITH THE TAG #100daysofmaking AND REBLOGS EVERY
//POST IT IDENTIFIES EVERY NIGHT AT MIDNIGHT.

//IMPORT NECESSARY LIBRARIES
var tumblr = require('tumblr.js');
var fs = require('fs');
var request = require('request');

//GET THE CURRENT TIME FOR WHEN THE BOT STARTS
var currentdate = new Date(); 
var datetime = currentdate.getDate() + "/"
                + (currentdate.getMonth()+1)  + "/" 
                + currentdate.getFullYear() + " @ "  
                + currentdate.getHours() + ":"  
                + currentdate.getMinutes() + ":" 
                + currentdate.getSeconds();

console.log("Bot starting at: "+ datetime);

//IMPORT API KEYS FROM SEPARATE CONFIG FILE
var config = require('./config'); //SEPARATE CONFIG FILE FOR STORING KEYS ETC

//INITIALIZE CLIENT WITH OBJECT FROM CONFIG FILE
var client = tumblr.createClient( config );

//KEEP TRACK OF WHAT DAY IT IS FOR THE CHALLENGE (BOT WRITTEN ON DAY 15)
var dayCounter = 15;

//CREATE A BOOLEAN TO ENSURE CODE ONLY RUNS ONCE A DAY
var waiting = true;

setInterval(checkTime, 1000*15);

function checkTime()
{
	console.log("checking time");
	var currentTime = new Date();
	console.log("checked at: " + currentTime);
	if (currentTime.getHours() === 6 && currentTime.getMinutes() === 58 && waiting == true)
	{
		setTimeout(reblogPosts, 1000 * 60);
		console.log("Waiting 1 minute before reblogging!");
		waiting = false;
	} else {
		//console.log("waiting");
	}

}

function reblogPosts()
{

	console.log("Reblogging class posts");

	// Make the request
	client.tagged('100daysofmaking', function (err, data) {

		if (err)
		{
			console.log(err);
		} else {

			//CHECK TODAY'S DATE
			var dateData = new Date(); 
			var year, month, day;
			year = dateData.getFullYear();

			//FORMAT THE DATE CORRECTLY FOR COMPARISION
			if (dateData.getMonth() < 9) month = '0' + (dateData.getMonth()+1).toString();
			else month = (dateData.getMonth()+1);
			if (dateData.getDate() < 9 ) day = '0' + (dateData.getDate()).toString();
			else day = dateData.getDate();

			var todayDate = year + '-' + month + '-' + day;

			// console.log(typeof(data));
			//console.log(data.length);

		 	// //UNCOMMENT TO SAVE THE JSON RESPONSE FILE
		 	// var json = JSON.stringify(data,null,2);
			//fs.writeFile("tagsData.json", json, function(){console.log("writing json response")});
		
			for (var i = 0; i < 5; i++)
			{
				//RETRIEVE TODAY'S DATE
				var dateToCheck = (data[i].date).toString();

				//SPLIT THE STRING TO GET DATE INTO THE FORMAT YYYY-MM-DD
				var splitDate = dateToCheck.split(" ");

				//THE DATE YYYY-MM-DD IS THE FIRST OBJECT IN THE ARRAY, CHECK THIS AGAINST THE DATE
				//FOUND IN THE RESPONSE.  IF THEY ARE THE SAME, THE POST IS FROM THE CURRENT DAY, SO
				//REPOST IT TO THE CLASS BLOG
				if(todayDate === splitDate[0]) 
				{
					var name = data[i].blog_name;

					//PRINT POST DETAILS TO THE CONSOLE
					console.log("-----");
					console.log("id: " + data[i].id);
					console.log("name: " + name);
					console.log("reblog_key: " + data[i].reblog_key);
					console.log("date: " + splitDate[0]);
					console.log("-----");

					//CREATE OBJECT WITH ALL THE POSTS INFORMATION
					var postData = {
						id: data[i].id,
						reblog_key: data[i].reblog_key,
						comment: "Day " + dayCounter + " - Post by " + name + " on " + splitDate[0]
					}

					//SUBMIT POST REQUEST
					client.reblog('100-daysofmaking-itp', postData, function postCallBack(err, data, response)
					{
						if (err) console.log(err);
						else console.log("successfully reposted");
					});
					
				} else {
					console.log("skipping old post");
				}
				
			}


		}

	});
	console.log("waiting 24 hours before next post");
	waiting = true;
	dayCounter++;
}
