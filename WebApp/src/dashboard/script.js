const sideMenu = document.querySelector("aside");
const menuBtn = document.querySelector("#menu-btn");
const closeBtn = document.querySelector("#close-btn");
const themeToggler = document.querySelector(".theme-toggler");

// show sidebar
menuBtn.addEventListener('click', () => {
    sideMenu.style.display = 'block';
})

// sidebar close
closeBtn.addEventListener('click', () => {
    sideMenu.style.display = 'none';
})

// change theme
themeToggler.addEventListener('click', () => {
    document.body.classList.toggle('dark-theme-variables');

    themeToggler.querySelector('span:nth-child(1)').classList.toggle('active');
    themeToggler.querySelector('span:nth-child(2)').classList.toggle('active');
})

// Mapping sidebar with main
document.addEventListener("DOMContentLoaded", function() {
    // Get all tab elements
    const tabs = document.querySelectorAll('.sidebar a');

    // Get all main sections
    const mains = document.querySelectorAll('main');

    // Add click event listener to each tab
    tabs.forEach((tab, index) => {
        tab.addEventListener('click', function(event) {
            event.preventDefault(); // Prevent default link behavior

            // Remove 'active' class from all tabs
            tabs.forEach((t) => t.classList.remove('active'));
            
            // Add 'active' class to the clicked tab
            this.classList.add('active');

            // Hide all main sections
            mains.forEach((main) => main.style.display = 'none');

            // Show the corresponding main section
            mains[index].style.display = 'block';
        });
    });
});
