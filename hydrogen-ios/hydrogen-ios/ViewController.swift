//
//  ViewController.swift
//  hydrogen-ios
//
//  Created by Patrick Kage on 4/29/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

import UIKit
import JavaScriptCore

var serverDetails: String = "";
var program: String = "";

class ViewController: UIViewController, UITextFieldDelegate, UITextViewDelegate {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        print("something is happening!\n")
        if (self.serverField != nil) {
            self.serverField.delegate = self;
            println("server details: \"\(serverDetails)\"");
        }

        if (self.editorText != nil) {
            self.editorText.delegate = self;
            if (program != "") {
                self.editorText.text = program;
            } else {
                program = self.editorText.text;
            }
        }

        if (self.execTitle != nil) {
            self.returnTextView.hidden = true;
            self.execImage.hidden = false;
            self.execFailureImage.hidden = true;
            handleRemoteExecution();
        }
    }

    func textFieldShouldReturn(textField: UITextField) -> Bool {
        println("textfield return delegate is running");
        serverDetails = self.serverField.text;
        textField.resignFirstResponder();
        return true;
    }

    func textViewDidChange(textView: UITextView) {
        println("changed text view");
        program = self.editorText.text;
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBOutlet weak var serverField: UITextField!
    @IBOutlet weak var editorText: UITextView!
    @IBOutlet weak var execImage: UIImageView!
    @IBOutlet weak var execTitle: UINavigationItem!
    @IBOutlet weak var returnTextView: UITextView!
    @IBOutlet weak var execFailureImage: UIImageView!


    @IBAction func serverFieldValueChanged(sender: AnyObject) {
        println("serverFieldValue: " + serverField.text);
    }

    func displayRemoteExecutionResults(results: String, failure: Bool) {
        println("displaying rexec results: string");
        self.execImage.hidden = true;
        self.execFailureImage.hidden = true;
        if (failure) {
            self.execFailureImage.hidden = false;
            self.execTitle.title = "Failed to contact server!";
        } else {
            self.execTitle.title = "Response:";
            self.returnTextView.hidden = false;
            self.returnTextView.text = results;
        }
    }

    func handleRemoteExecution() {
        // log some stuff (for posterity)
        println("remotely executing things on " + serverDetails);
        println("program: " + program);

        // make the url separately because swift is weird about constructing
        // NSURLs inside requests.
        var url = NSURL(string: "http://" + serverDetails + "/exec");

        // make our request handler
        var request = NSMutableURLRequest(URL: url!);
        // make said request handler POST
        request.HTTPMethod = "POST";

        // make an NSURLSession
        var session = NSURLSession.sharedSession();

        // make a throwaway error object
        var err: NSError?

        // create our request - first by serializing our data
        // step 1: split our program text into an array of strings
        //         along the newlines using the global split() fn.
        //         (why is that global? what's the logic behind that?)
        var tmpProg = program;
        tmpProg = tmpProg.stringByReplacingOccurrencesOfString("\u{2028}", withString: "\n", options: NSStringCompareOptions.LiteralSearch, range: nil);
        var progArr = split(tmpProg) {$0 == "\n"}
        // step 2: make our object (to be serialized)
        var params = ["program":progArr] as Dictionary<String, Array<String> >
        // step 3: serialize it and set the set the body to the resulting json
        request.HTTPBody = NSJSONSerialization.dataWithJSONObject(params, options: nil, error: &err);

        // add some content-type data to the header
        request.addValue("application/json", forHTTPHeaderField: "Content-Type");

        execTitle.title = "Waiting for response from server...";

        // make our task
        var task = session.dataTaskWithRequest(request, completionHandler: {data, response, error -> Void in
            // log for posterity
            println("response: \(response)");

            // run this on the main thread, otherwise swift gets all huffy about resizing the UITextField
            dispatch_async(dispatch_get_main_queue(), {
                // get the string data out of the response
                var strData = NSString(data: data, encoding: NSUTF8StringEncoding);

                // swift's json support is *awful* at best, and it was a huge headache
                // believe it or not, this is easier.

                // create a javascript context
                var jscontext = JSContext();
                // load in the function that'll make the json pretty
                jscontext.evaluateScript("var formatJSON = function(input) {input = JSON.parse(input);var ret = \"\";for (var c = 0; c < input.length; c++) {ret += input[c] + \"\\n\"} return ret}");
                // get the function pointer to the json formatter
                let formatFunc = jscontext.objectForKeyedSubscript("formatJSON");
                // run the function with some args
                let formatResults = formatFunc.callWithArguments([(strData as! String)]);

                // display the result
                self.displayRemoteExecutionResults("\(formatResults)", failure: ("\(formatResults)" == "undefined"));
            });
        });


        task.resume();


    }
}

