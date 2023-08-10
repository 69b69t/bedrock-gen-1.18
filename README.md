# bedrock-gen-1.18
bedrock generation code so people dont have to wade through minecraft's spaghetti source code like i did. the O stands for Overworld, and the N stands for Nether. all these output the bedrock patterns for the seed 694201337

compile with `gcc md5.c bedrockO118.c -o bedrockO118` (compiles for overworld bedrock)

or

compile with `gcc bedrockN118.c -o bedrockN118` (compiles for nether bedrock; roof and floor)

outputs a grid of text but its pretty easy to change
