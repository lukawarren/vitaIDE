<template>
    <div id="app" class="md-layout">
        <Panel
            title="Source"
            class="md-layout-item md-size-70 md-small-size-100"
        >
            <Source />
        </Panel>
        <Panel
            title="Console"
            class="md-layout-item md-size-30 md-small-size-100"
        >
            <Console />
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
}
</style>
