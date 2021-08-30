<template>
    <div>
        <md-card md-with-hover>
            <md-toolbar md-elevation>
                <div class="md-title">{{ title }}</div>
            </md-toolbar>

            <md-card-content>
                <slot></slot>
            </md-card-content>
        </md-card>
    </div>
</template>

<script>
export default {
    name: "Panel",
    props: ["title"],
};
</script>

<style scoped lang="scss">
.md-card {
    margin-top: 8px;
    margin-bottom: 8px;
    margin-left: 4px;
    margin-right: 4px;
}
</style>
