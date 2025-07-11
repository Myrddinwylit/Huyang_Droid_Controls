/*
 * Name          : joy.js
 * @author       : Roberto D'Amico (Bobboteck)
 * Last modified : 09.06.2020
 * Revision      : 1.1.6
 *
 * Modification History:
 * Date         Version     Modified By     Description
 * 2021-12-21   2.0.0       Roberto D'Amico New version of the project that integrates the callback functions, while
 * maintaining compatibility with previous versions. Fixed Issue #27 too,
 * thanks to @artisticfox8 for the suggestion.
 * 2020-06-09   1.1.6       Roberto D'Amico Fixed Issue #10 and #11
 * 2020-04-20   1.1.5       Roberto D'Amico Correct: Two sticks in a row, thanks to @liamw9534 for the suggestion
 * 2020-04-03               Roberto D'Amico Correct: InternalRadius when change the size of canvas, thanks to
 * @vanslipon for the suggestion
 * 2020-01-07   1.1.4       Roberto D'Amico Close #6 by implementing a new parameter to set the functionality of
 * auto-return to 0 position
 * 2019-11-18   1.1.3       Roberto D'Amico Close #5 correct indication of East direction
 * 2019-11-12   1.1.2       Roberto D'
 */
(function(window) { // Self-executing anonymous function to create a scope
    function JoyStick(stickID, options, cb) {
        var canvas = document.getElementById(stickID);
        var ctx = canvas.getContext('2d');
        var isDragging = false;
        var currentX = 0;
        var currentY = 0;
        var centerX = canvas.width / 2;
        var centerY = canvas.height / 2;
        var stickRadius = options.stickRadius || 40;
        var baseRadius = options.size / 2 || 75; // Using options.size for consistency

        // Default colors
        var baseColor = options.baseColor || 'rgba(100, 100, 100, 0.5)';
        var stickColor = options.stickColor || 'rgba(0, 0, 0, 0.5)';

        // Drawing the joystick base
        function drawBase() {
            ctx.beginPath();
            ctx.arc(centerX, centerY, baseRadius, 0, Math.PI * 2, true);
            ctx.fillStyle = baseColor;
            ctx.fill();
            ctx.closePath();
        }

        // Drawing the joystick stick
        function drawStick() {
            ctx.beginPath();
            ctx.arc(currentX, currentY, stickRadius, 0, Math.PI * 2, true);
            ctx.fillStyle = stickColor;
            ctx.fill();
            ctx.closePath();
        }

        // Clear the canvas and redraw
        function redraw() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            drawBase();
            drawStick();
        }

        // Initialize stick position
        currentX = centerX;
        currentY = centerY;
        redraw();

        // Event listeners
        canvas.addEventListener('mousedown', onMouseDown);
        canvas.addEventListener('mousemove', onMouseMove);
        canvas.addEventListener('mouseup', onMouseUp);
        canvas.addEventListener('mouseout', onMouseUp); // Release stick if mouse leaves canvas

        canvas.addEventListener('touchstart', onTouchStart, { passive: false });
        canvas.addEventListener('touchmove', onTouchMove, { passive: false });
        canvas.addEventListener('touchend', onTouchEnd);

        function onMouseDown(e) {
            isDragging = true;
            updateStick(e.clientX, e.clientY);
        }

        function onMouseMove(e) {
            if (isDragging) {
                updateStick(e.clientX, e.clientY);
            }
        }

        function onMouseUp() {
            isDragging = false;
            resetStick();
        }

        function onTouchStart(e) {
            isDragging = true;
            e.preventDefault(); // Prevent scrolling
            updateStick(e.touches[0].clientX, e.touches[0].clientY);
        }

        function onTouchMove(e) {
            if (isDragging) {
                e.preventDefault(); // Prevent scrolling
                updateStick(e.touches[0].clientX, e.touches[0].clientY);
            }
        }

        function onTouchEnd() {
            isDragging = false;
            resetStick();
        }

        function updateStick(clientX, clientY) {
            var rect = canvas.getBoundingClientRect();
            var mouseX = clientX - rect.left;
            var mouseY = clientY - rect.top;

            var deltaX = mouseX - centerX;
            var deltaY = mouseY - centerY;
            var distance = Math.sqrt(deltaX * deltaX + deltaY * deltaY);

            var maxMoveStick = baseRadius - stickRadius; // Max distance the stick can move from center

            if (distance > maxMoveStick) {
                var angle = Math.atan2(deltaY, deltaX);
                currentX = centerX + maxMoveStick * Math.cos(angle);
                currentY = centerY + maxMoveStick * Math.sin(angle);
            } else {
                currentX = mouseX;
                currentY = mouseY;
            }

            redraw();
            callCallback();
        }

        function resetStick() {
            currentX = centerX;
            currentY = centerY;
            redraw();
            callCallback();
        }

        function callCallback() {
            var maxMoveStick = baseRadius - stickRadius;
            var relativeX = ((currentX - centerX) / maxMoveStick) * 100;
            var relativeY = ((currentY - centerY) / maxMoveStick) * 100;

            // Clamp values to -100 to 100 for consistency, handling floating point inaccuracies
            relativeX = Math.min(100, Math.max(-100, relativeX));
            relativeY = Math.min(100, Math.max(-100, relativeY));

            cb({ x: relativeX, y: relativeY, isDragging: isDragging });
        }

        // Public method to check if stick is currently being dragged
        this.isDragging = function() {
            return isDragging;
        };

        // Added to fix the specific methods your javascript.js calls
        // These are not ideal, as the callback should provide the data,
        // but to ensure compatibility with your existing sendNeckUpdate/sendBodyUpdate
        // which were using JoyNeckX and JoyNeckY which were globally declared and updated by the callback.
        // The original joy.js had GetX/GetY methods, but its callback structure is different.
        // This is a workaround to make your current javascript.js happy.
        this.GetX = function () {
            let maxMoveStick = baseRadius - stickRadius;
            let valX = currentX - centerX;
            return (100 * (valX / maxMoveStick)).toFixed(0);
        };
        this.GetY = function () {
            let maxMoveStick = baseRadius - stickRadius;
            let valY = currentY - centerY;
            return ((100 * (valY / maxMoveStick)) * -1).toFixed(0); // Assuming Y is inverted for your robot
        };
        this.GetValues = function () {
            return { x: parseInt(this.GetX()), y: parseInt(this.GetY()) };
        };
        // End of compatibility additions
    }

    if (!Number.prototype.toFixed) {
        Number.prototype.toFixed = function(precision) {
            var power = Math.pow(10, precision || 0);
            return (Math.round(this * power) / power).toString();
        };
    }

    // CRITICAL FIX: Expose JoyStick to the global scope
    window.JoyStick = JoyStick;

})(window);