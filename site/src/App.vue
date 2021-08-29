<template>
    <div id="app" class="md-layout">
        <Panel title="Source" class="md-size-70 md-small-size-100">
            <Source />
        </Panel>
        <Panel title="Console" class="md-size-30 md-small-size-100">
            Python 2.7.18 (default, Jul 14 2021, 08:11:37)<br />
            [GCC 10.2.1 20210110] on linux2<br />
            Type "help", "copyright", "credits" or "license" for more information.<br />
            >>> print("I was too lazy to find a Lua prompt")<br />
            >>>
        </Panel>
    </div>
</template>

<script>
import Panel from "./components/Panel.vue";
import Source from "./components/Source.vue";

export default {
    name: "App",
    components: {
        Panel,
        Source,
    },
};
</script>

<style lang="scss">
#app {
    margin-left: 8px;
    margin-right: 8px;
    
    // For equal height panels
    display: flex;
    align-items: stretch;    
}
</style>
