# Retrive info from text and markdown files with Parrot

# Install
- Download and save parrot to a directory of your choice
- Run install script inside its own directory
- To give a file to parrot without providing or --folder --filename each time:
  Create a link to ./Files folder with parrot-link:
  ```bash
  parrot-link <path to the file>
  ```

# Parrot Files
- Any file of text with a series of alternating text lines and multiple hashtags to retrive it
- ## Ex.
  ```
  # First Topic

  This is the first topic text
  You may want to retrive it with 
  parrot "This FileName" "First Topic"
  
  # Second Topic # 2nd t.
  
  You may retrive this with 
  parrot "This FileName" "2nd t."
  ```
# Features
- No need to specify full hashtag
- Deals also with multiple reference to same hashtag
- By default is case insensitive
- With -h options you get full info.
