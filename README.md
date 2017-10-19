# Broken, go bother https://github.com/UFeindschiff/devchat-gui for an updated version

#Intro
DevchatGUI (or dcgui) is a graphical client for the X-Devchat of EGOSOFT. It's based on Gtk+ and offers (or is planned to offer) several improvements over the normal browser client, e.g. tabbed PMs and several graphical improvements (Avatars in the user list, themeable colors etc.).

#Installation
See INSTALL.

#User's Guide
Well... how do you use a chat client? Most features should be self-explanatory. Notable difference to the browser client (or many other chats) is the user list: The PM button opens a private conversation in a new tab, clicking on the username writes "@$Username" into the text box, and clicking on an avatar open the user's forum profile.
Note for Windows® users: You can't use Alt+NumblockCharcode to enter special chars (like ™,©,...) in GTK+ applications. However, you can enter them by first pressing (and releasing) Ctrl+Shift+U, typing the (Unicode) hex codepoint (e.g. 1e9e for ẞ) and then pressing space (this works under *nix too).

#Moderator's Guide to Wor... Chat Domination
For obvious reasons, the normal kick/silence script doesn't work. You can kick/silence users of dcgui by writing !kick $Username / !silence $Username (Username incl. spaces, not &nbsp;) in the main chat or just !kick / !silence in a PM. This also works when embedded in <!-- -->, so you can include it into your normal kickscript – Which leads me to the next specific feature of dcgui: Normally, even Greenies can't write plain HTML, the messages will be escaped like those of any other user. To send raw HTML code, activate the "raw mode" check box before sending a post.
