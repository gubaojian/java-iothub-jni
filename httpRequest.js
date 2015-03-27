/*
 * httpRequest.js
 * v0.1
 * 2015
 *
 * Copyright 2015 University of Helsinki
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at:
 * 	http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, 
 * software distributed under the License is distributed on an 
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
 * either express or implied.
 * See the License for the specific language governing permissions 
 * and limitations under the License.
 */

/*
 Events:
 * onloadstart
 * onprogress
 * onabort
 * onload
 * ontimeout
 * onloadend
 ** onreadystatechange
*/

/* Hooks for using XMLHttpRequest from JSOO */
var location = {};
location["host"] = "127.0.0.1:8080";
location["hostname"] = "127.0.0.1";
location["protocol"] = "http:";
location["href"] = "http://127.0.0.1/applications/";
location["origin"] = "http://127.0.0.1:8080";
location["pathname"] = "/applications/";
location["port"] = "8080"; //return empty if 80 or 443
location["hash"] = "";
location["search"] = "";

//Math.imul emulation from Mozilla dev
//https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/imul
Math.imul = Math.imul || function(a, b) {
  var ah  = (a >>> 16) & 0xffff;
  var al = a & 0xffff;
  var bh  = (b >>> 16) & 0xffff;
  var bl = b & 0xffff;
  // the shift by 0 fixes the sign on the high part
  // the final |0 converts the unsigned value into a signed value
  return ((al * bl) + (((ah * bl + al * bh) << 16) >>> 0)|0);
};

var XMLHttpRequest = function() {
  	
  	var self = this;
  	
	// Constants
	this.UNSENT = 0;
  	this.OPENED = 1;
  	this.HEADERS_RECEIVED = 2;
  	this.LOADING = 3;
  	this.DONE = 4;
  	
  	//Set the ready state variable
  	var readyState = this.UNSENT; //readonly
  	
  	// default ready state change handler in case one is not set or is set late
  	this.onreadystatechange = null;
  	
  	//Request functions and attributes
  	var request;
  	
  	// check http://www.w3.org/TR/2014/WD-XMLHttpRequest-20140130/
  	this.open = function(method, url, async, username, password) {
        this.abort(); //First abort any current task
        this.settings = {
      		"method": method,
      		"url": url.toString(),
      		"async": (typeof async !== "boolean" ? true : async),
      		"username": username || null,
      		"password": password || null
    	};
    };
    this.setRequestHeader = function(header, value) {
    
    };
    this.send = function(data) {
    	var hasData = typeof data !== 'undefined';
    	//Now I will try to use some Java method to call the thing
    	var resp = native_request_send(this.settings.method, this.settings.url);
    	if (resp) {
    		this.readyState = this.DONE;
    		this.status = 200;
    		this.responseText = resp["response"];
    		if (this.onreadystatechange) {
    			this.onreadystatechange()
    		}
    	} else { print('Failure'); }
    };
    this.abort = function() {
    
    };
    
    //Response functions and attributes
    var response;
  	this.responseText = "";
  	this.responseXML = "";
  	this.status = null;
  	this.statusText = null;
  	
  	this.getResponseHeader = function(header) {
  	}
  	this.getAllResponseHeaders = function() {
  	}
  	this.overrideMimeType = function(mime) {
  	}  	
}
