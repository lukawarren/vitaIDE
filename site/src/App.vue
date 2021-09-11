<template>
    <div id="app" class="md-layout">
        <Panel
            title="Source"
            class="md-layout-item md-size-80 md-small-size-100"
        >
            <Source v-on:send="send" />
        </Panel>
        <Panel
            title="Console"
            class="md-layout-item md-size-20 md-small-size-100"
        >
            <Console :code="displayCode" />
        </Panel>
    </div>
</template>

<script>
import Panel from "./components/Panel.vue";
import Source from "./components/Source.vue";
import Console from "./components/Console.vue";

export default {
    name: "App",
    components: {
        Panel,
        Source,
        Console,
    },
    created() {
        window.setInterval(() => {
            if (this.code != "") {
                // Submit to vita's lua server (see C++ code)
                var req = new XMLHttpRequest();
                req.open("POST", "http://" + "192.168.1.192" + ":1010/");
                req.addEventListener("load", () => {
                    this.displayCode = req.responseText;
                });
                req.send("LUA " + this.code);

                this.code = "";
            }
        }, 1000);
    },
    methods: {
        send(code) {
            this.code = code;
        },
    },
    data: () => ({
        code: "",
        displayCode: "",
    }),
};
</script>

<style lang="scss">
@import "~vue-material/dist/theme/engine"; // Import the theme engine

@include md-register-theme(
    "default",
    (
        primary: md-get-palette-color(blue, A200),
        accent: md-get-palette-color(red, A200),
        theme: dark,
    )
);

@import "~vue-material/dist/theme/all"; // Apply the theme

#app {
    margin-left: 8px;
    margin-right: 8px;

    // For equal height panels
    display: flex;
    align-items: stretch;

    min-height: 100vh;
}
</style>
