let location = "start";

function startGame() {
  displayLocation();
}

function displayLocation() {
  if (location === "start") {
    display("You are at the start. There are paths to the north and east.");
    display("What do you do? (north/east/quit)");
  } else if (location === "north") {
    display("You are in the north. There is a path to the south.");
    display("What do you do? (south/quit)");
  } else if (location === "east") {
    display("You are in the east. There is a path to the west.");
    display("What do you do? (west/quit)");
  }
}

function move(direction) {
  if (location === "start") {
    if (direction === "north") {
      location = "north";
    } else if (direction === "east") {
      location = "east";
    }
  } else if (location === "north") {
    if (direction === "south") {
      location = "start";
    }
  } else if (location === "east") {
    if (direction === "west") {
      location = "start";
    }
  }
  displayLocation();
}

function display(text) {
  console.log(text);
}

function handleInput(input) {
  console.log(`> ${input}\n`);
  if (input === "north" || input === "east" || input === "south" || input === "west") {
    move(input);
  } else if (input === "quit") {
    display("Thanks for playing!");
  } else {
    display("Invalid input. Please try again.");
  }
}

startGame();

// Example usage (in a real environment, you would get input from the user)
handleInput("north");
handleInput("south");
handleInput("east");
handleInput("west");
handleInput("quit");