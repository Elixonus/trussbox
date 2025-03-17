class Joint {
    constructor(mass, position, velocity) {
        this.mass = mass;
        this.position = position;
        this.velocity = velocity;
    }
}

class Member {
    constructor(joint1, joint2, stiffness, length0, dampening) {
        this.joint1 = joint1;
        this.joint2 = joint2;
        this.stiffness = stiffness;
        this.length0 = length0;
        this.dampening = dampening;
    }
}

class Support {
    constructor(joint, axes, position) {
        this.joint = joint;
        this.axes = axes;
        this.position = position;
    }
}

class Load {
    constructor(joint, force) {
        this.joint = joint;
        this.force = force;
    }
}

let mode = "joint";
let joints = [];
let members = [];
let supports = [];
let loads = [];

const canvas = document.getElementById("canvas");

let mousePosition = [0.0, 0.0]

canvas.addEventListener("click", mouseClick);

function mouseClick(event) {
    if(mode === "joint") {

    }
}

window.addEventListener("mousemove", mouseMove);

function mouseMove(event) {
    let rectangle = canvas.getBoundingClientRect();
    mousePosition[0] = (event.clientX - rectangle.left) / (rectangle.right - rectangle.left) - 0.5;
    mousePosition[1] = (event.clientY - rectangle.top) / (rectangle.bottom - rectangle.top) - 0.5;
}
