# Welcome to GitHub!
*git* is a version control software, and *github* is a website that allows one to store their repositories remotely. This is just an example of a repo on github. git enables many people to work on one project at the same time.

# How Do I Join and Cooperate?

## First of All...
- You need to [register](https://github.com/join) an account on github. It would not take very long!
- You need to install git on your computer, which can be downloaded [here](https://git-scm.com/downloads).

## Now, You Need To Clone the Repo At Your Computer!
- Create a folder where you want to put the repo on your computer, and run a command line window there (to do this quickly on Windows, hold ```Shift``` and right-click in the folder, then select "Open Windows PowerShell Here")
- In the command line window, type
```
	git clone https://github.com/Linydong/Bayesian_Classifier_Project.git
```
Now you should be able to view the project on your own computer!

## How Do I Modify the Code?
Feel free to change the code locally. If you feel like your code is helpful and need to be known by other group members, you should **push** your changes to the remote repo. To do this, you have to be a collaborator of this project. This has to be approved by the owner of the repo. She would send an **invitation request** to you. After your acceptance, you can push your code to the remote repo. When the invitation is sent, you should be able to check it out [here](https://github.com/Linydong/Bayesian_Classifier_Project/invitations).

When you use git for the first time, you need to set up your email and name. To do this, run
```
	git config --global user.email "example@example.com"
	git config --global user.name "John Doe"
```

To let git know that you have changed your code, in the command line window, run
```
	git add *
```
Now you need to explain why you modified the code, usually with a short comment.
```
	git commit -m "some short comment"
```
Then, we can push the change to the remote repo so that everyone can see
```
	git push origin master
```