<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcutoxicgasdatainfo */

$this->title = 'Create Hcutoxicgasdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcutoxicgasdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutoxicgasdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
