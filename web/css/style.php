.container {
    height:      100%;
    font-family: Arial;
}

h1, h2, h3, h4, h5, h6 {
    margin:      revert;
    font-size:   revert;
    font-weight: revert;
}

h2 {
    margin-top:     40px;
    margin-bottom:  20px;
}

.container p {
    margin-top:     15px;
    margin-bottom:  15px;
}

p code {
    background-color:   #eeeeee;
    border:             1px dotted #cccccc;
    padding:            2px;
    margin:             0px;
    font-size:          0.8em;
}

li code {
    background-color:   #eeeeee;
    border:             1px dotted #cccccc;
    padding:            2px;
    margin:             0px;
    font-size:          0.8em;
}

pre {
    background-color:   #eeeeee;
    border:             1px solid LightGrey;
    margin:             20px;
    padding:            20px;
    font-size:          0.8em;
    white-space:        break-spaces;
    overflow-wrap:      break-word;
}

.container ul {
    list-style-type:    revert;
    padding:            revert;
}

.container a {
    text-decoration: none;
    color:           Blue;
}

p a:hover {
    text-decoration: underline;
}

blockquote {
    background-color:   LightYellow;
    border-left:        6px solid Orange;
    padding:            5px;
    margin-right:       0px;
}

@media only screen and (max-width: 600px) {
    td {
    display: block;
    width: 100%;
    }
}