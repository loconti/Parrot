# Retrive info from text files with Parrot

# Install
- Download and save parrot to a directory of your choice
- Set the correct directory in parrot.sh
- Link the script to /usr/local/bin/parrot
```bash
ln -s "<parrot folder>/parrot.sh" /usr/local/bin/parrot
```
- Link your parrot files to Files directory with
```bash
ln -s "<your parrot file>" "$(parrot --folder-path)<your parrot file>"
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
