<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscstadatainfo */

$this->title = 'Create Hcubfscstadatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcubfscstadatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfscstadatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
